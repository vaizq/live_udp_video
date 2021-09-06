// Server send video to client

#include <opencv2/opencv.hpp>
#define ENET_IMPLEMENTATION
#include "enet.h"
#include "config.h"

#include <iostream>
#include <cstdlib>


int main(int argc, char** argv)
{
  if (argc != 3) { // Test for correct number of parameters
    std::cerr << "Usage: " << argv[0] << " <Host> <Server Port>"<< std::endl;
    exit(1);
  }
  char* peerAddress = argv[1];
  unsigned int servPort = atoi(argv[2]); // First arg:  local port 

  if(enet_initialize() != 0)
  {
    std::cerr << "An error occured while initializing ENet" << std::endl;
    exit(1);
  }

 
  ENetHost* client = enet_host_create(NULL, 1, 2, 0, 0); 
  
  if(client == NULL)
  {
    std::cerr << "An error occured while trying to create client host." <<
      std::endl;
    exit(1);
  }

  ENetAddress address;
  
  enet_address_set_host(&address, peerAddress); 
  address.port = servPort;
  ENetPeer* peer = enet_host_connect(client, &address, 2, 0);

  if(peer == NULL)
  {
    std::cerr << "No available peer for initialiating an ENet connection." <<
      std::endl;
    exit(1);
  }

  ENetEvent event;
  // Wait up to 5 seconds for the connection to succeed
  if(enet_host_service(client, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT)
  {
    std::cout << "Connection to " << peerAddress << " succeeded." << std::endl;
  }
  else
  {
    enet_peer_reset(peer);
    std::cout << "Connection to " << peerAddress << " failed." << std::endl;
  }


  // Opencv stuff
  cv::VideoCapture cap(0); // Grab the camera
  if(!cap.isOpened())
  {
    std::cerr << "OpenCV failed to open camera" << std::endl;
    exit(1);
  }
 // cv::namedWindow("send", cv::WINDOW_AUTOSIZE);

  cv::Mat frame, send;
  std::vector< uchar > encoded;
  std::vector < int > compression_params;
  compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
  compression_params.push_back(ENCODE_QUALITY);


  while(1)
  {
    // Read image from webcam, encode it and send
    
    cap >> frame;
    cv::resize(frame, send, cv::Size(FRAME_WIDTH, FRAME_HEIGHT),
        0, 0, cv::INTER_LINEAR);
    cv::imencode(".jpg", send, encoded, compression_params);
  
    ENetPacket* img_packet = enet_packet_create(encoded.data(),
                                                encoded.size() * sizeof(uchar),
                                                ENET_PACKET_FLAG_RELIABLE);
    if(img_packet == NULL)
    {
      std::cout << "Failed to create packet." << std::endl;
      continue;
    } 
    
    enet_peer_send(peer, 0, img_packet);

    if(enet_host_service(client, &event, 0) > 0)
    {
      // Some event happened
    }

    cv::waitKey(10);
  }

  atexit(enet_deinitialize);
  enet_host_destroy(client);

  return 0;
}






