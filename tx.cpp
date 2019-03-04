
#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <chrono>
#include <complex>
#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
//#include <cstring>








void send_from_file(uhd::tx_streamer::sptr tx_stream, size_t samps_per_buff)
{
    uhd::tx_metadata_t md;
    md.start_of_burst = false;
    md.end_of_burst   = false;
    //std::ifstream infile("hundred.bin");
    //std::ifstream myFile ("frames.bin", ios::in | ios::binary);
    //float sample;
    //std::vector<float> buff;
    //std::string file ("nprs_wave.bin");
    //std::ifstream infile(file.c_str(), std::ifstream::binary);
    //std::complex<float> x;
    //std::ifstream infile;
    //std::vector<std::complex<float>>buff(samps_per_buff);
    std::vector<std::complex<float>>duff(9600);
    std::vector<std::complex<float>>muff(192000);
    std::vector<std::complex<float>>buff(samps_per_buff);
    size_t num_tx_samps = 9600;
    std::complex<float> x;
    long int j = 0,c=0;
    int size;
    std::string file("test_sig.bin");
    std::fstream infile ;
	infile.open(file, std::ios::in | std::ios::binary);
	infile.seekg(0, std::ios::end);
	size = (int) infile.tellg();
	infile.seekg(0, std::ios::beg);
	while(infile.tellg() < size){
	    infile.read((char*)&buff.front(), buff.size()* sizeof(x));
// read one subframe from file and then write it to an array
		for( int i = 0; i < 1920; i++){
		        muff[j]= buff[i];
		        j++;

		}


	}
    infile.close();
    

	    while(c < 192000){
	    	for(int i = 0; i < 9600; i++)
	    	{
	    		duff[i] = muff[c];

	    		c++;

	    	}
            if (c == 9600 ) {
                md.start_of_burst   = true;
                tx_stream->send(&duff.front(), num_tx_samps, md);
            }
            
	    	else if (c == 192000) {
                md.end_of_burst   = true;
                tx_stream->send(&duff.front(), num_tx_samps, md);

	    	}
	    	else
	    	{
                tx_stream->send(&duff.front(), num_tx_samps, md);
                md.start_of_burst   = false;

	    	}
        }
    

    
/*


    while (infile.is_open() and not md.end_of_burst) {
        //std::cout << "Hello"<< std::endl;
       // md.end_of_burst = infile.eof();

        infile.read((char*)&buff.front(), buff.size() * (sizeof(x)));
        size_t num_tx_samps = size_t(infile.gcount() / (sizeof(x)));
        std::cout << num_tx_samps << std::endl;

        md.end_of_burst = infile.eof();

        tx_stream->send(&buff.front(), num_tx_samps, md);
    }

     infile.close();

*/
 // for(size_t i = 0; i < buff.size(); i++)
    //{
      //  std::cout << buff[i] << std::endl;
        //std::cout << i << std::endl;
    //}
 
    

    //tx_stream->send(b, c, md);

    
}



int UHD_SAFE_MAIN(int argc, char *argv[]) {
    uhd::set_thread_priority_safe();

    std::string device_args("addr=192.168.10.2");
    std::string subdev("A:0");
    std::string ant("TX/RX");
    std::string ref("internal");

    double rate(1.92e6);
    double freq(915e6);
    double gain(10);
    double bw(1e6);

    //create a usrp device
    std::cout << std::endl;
    std::cout << boost::format("Creating the usrp device with: %s...") % device_args << std::endl;
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(device_args);

    // Lock mboard clocks
    std::cout << boost::format("Lock mboard clocks: %f") % ref << std::endl;
    usrp->set_clock_source(ref);

    //always select the subdevice first, the channel mapping affects the other settings
    std::cout << boost::format("subdev set to: %f") % subdev << std::endl;
    usrp->set_tx_subdev_spec(subdev);
    std::cout << boost::format("Using Device: %s") % usrp->get_pp_string() << std::endl;

    //set the sample rate
    if (rate <= 0.0) {
        std::cerr << "Please specify a valid sample rate" << std::endl;
        return ~0;
    }

    // set sample rate
    std::cout << boost::format("Setting TX Rate: %f Msps...") % (rate / 1e6) << std::endl;
    usrp->set_tx_rate(rate);
    std::cout << boost::format("Actual TX Rate: %f Msps...") % (usrp->get_tx_rate() / 1e6) << std::endl << std::endl;

    // set freq
    std::cout << boost::format("Setting TX Freq: %f MHz...") % (freq / 1e6) << std::endl;
    uhd::tune_request_t tune_request(freq);
    usrp->set_tx_freq(tune_request);
    std::cout << boost::format("Actual TX Freq: %f MHz...") % (usrp->get_tx_freq() / 1e6) << std::endl << std::endl;

    // set the rf gain
    std::cout << boost::format("Setting TX Gain: %f dB...") % gain << std::endl;
    usrp->set_tx_gain(gain);
    std::cout << boost::format("Actual RX Gain: %f dB...") % usrp->get_tx_gain() << std::endl << std::endl;

    // set the IF filter bandwidth
    std::cout << boost::format("Setting TX Bandwidth: %f MHz...") % (bw / 1e6) << std::endl;
    usrp->set_tx_bandwidth(bw);
    std::cout << boost::format("Actual TX Bandwidth: %f MHz...") % (usrp->get_tx_bandwidth() / 1e6) << std::endl << std::endl;

    // set the antenna
    std::cout << boost::format("Setting TX Antenna: %s") % ant << std::endl;
    usrp->set_tx_antenna(ant);
    std::cout << boost::format("Actual TX Antenna: %s") % usrp->get_tx_antenna() << std::endl << std::endl;

    // set the streamer


    //std::string file;
    size_t spb(1920);
    //size_t channel(0);
    //std::string file("ten.bin");
    std::string cpu_format("fc32");
    std::string otw("sc16");
    //std::string channel("0");
    //std::vector<size_t> channel_nums;

    uhd::stream_args_t stream_args(cpu_format, otw);
    stream_args.channels = {0};
    uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(stream_args);

    

   for(size_t i = 0; i < 3; i++)
    {
        send_from_file(tx_stream, spb);
    }
   
     //send_from_file(tx_stream, spb);

   

    // finished
    std::cout << std::endl << "Sent & Done!" << std::endl << std::endl;










    return EXIT_SUCCESS;
}

