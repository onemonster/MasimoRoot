cmake_minimum_required(VERSION 3.16.3)
project(MasimoRoot)

set(CMAKE_CXX_STANDARD 17)

add_executable(MasimoRoot main.cpp Socket.cpp Socket.h SerialPort.cpp SerialPort.h IAP.cpp IAP.h Root.cpp Root.h response_message/ResponseMessage.cpp response_message/ResponseMessage.h response_message/AckResponse.cpp response_message/AckResponse.h response_message/NakResponse.cpp response_message/NakResponse.h response_message/ProtocolRevisionResponse.cpp response_message/ProtocolRevisionResponse.h response_message/DataGroupResponse.cpp response_message/DataGroupResponse.h response_message/DataGroup.h IAPClient.cpp IAPClient.h response_message/UnknownResponse.cpp response_message/UnknownResponse.h)