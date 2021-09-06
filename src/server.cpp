// Client receives video from server

#include <opencv2/opencv.hpp>
#define ENET_IMPLEMENTATION
#include "enet.h"

#include <iostream>
#include <cstdlib>


int main(int argc, char** argv)
{
  if (argc != 2) { // Test for correct number of parameters
    std::cerr << "Usage: " << argv[0] << " <Server Port>" << std::endl;
    exit(1);
  }
  unsigned int servPort = atoi(argv[1]); // First arg:  local port

  if(enet_initialize() != 0)
  {
    std::cerr << "An error occured while initializing ENet" << std::endl;
    exit(1);
  }

  ENetAddress address;

  address.host = ENET_HOST_ANY;
  address.port = servPort;

  ENetHost* server = enet_host_create(&address, 2, 2, 0, 0);

  if(server == NULL)
  {
    std::cerr << "An error occured while trying to create an ENet server host." <<
      std::endl;
    exit(1);
  }
  std::cout << "Started server" << std::endl;
  cv::namedWindow("recv", cv::WINDOW_AUTOSIZE);
  
  cv::Mat frame;
  while(1)
  {
    // Check to receive a package
    ENetEvent event;
    while(enet_host_service(server, &event, 0) > 0)
    {
      // Receive data in a buffer, decode it to Mat and show it
      switch(event.type)
      {
      case ENET_EVENT_TYPE_CONNECT:
        std::cout << "A new client connected." << std::endl;
        break;

      case ENET_EVENT_TYPE_RECEIVE:
        std::cout << "Received some package." << std::endl;

        cv::Mat rawData = cv::Mat(1, event.packet->dataLength,
                          CV_8UC1, event.packet->data);
        frame = cv::imdecode(rawData, cv::IMREAD_COLOR);

        enet_packet_destroy(event.packet);
        break;
      }
    }
    
    // show image
    if(frame.size().width > 0)
      cv::imshow("recv", frame);
    cv::waitKey(10);
  }
  
  atexit(enet_deinitialize);
  enet_host_destroy(server);
  return 0;
}






