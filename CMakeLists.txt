CMAKE_MINIMUM_REQUIRED( VERSION 2.8 ) 
PROJECT( hnode2-irrigation-device ) 

SET( MAJOR_VERSION 2 )
SET( MINOR_VERSION 2 )
SET( PATCH_VERSION 0 )
SET( RELEASE_VERSION 0 )

SET( DAEMON_NAME hnirrigationd )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake) 

# Default the install prefix to just /usr
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX /usr CACHE PATH "Default to /usr install" FORCE )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

SET( VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} )
SET( DEBVERSION ${VERSION}-${RELEASE_VERSION} )

ADD_DEFINITIONS( -DVERSION=${VERSION} )

SET( HNIRRIGATIOND_SRC
     ${CMAKE_SOURCE_DIR}/src/daemon/hnirrigationd.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationDevice.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationSchedule.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIDActionRequest.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNSWDPacketClient.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNSWDStatus.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationZone.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationPlacement.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationModifier.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationSequence.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationInhibit.cpp     
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationTypes.cpp
)

SET( HNI_TEST_SCHEDULER_SRC
     ${CMAKE_SOURCE_DIR}/src/daemon/hni_test_scheduler.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationSchedule.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationZone.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationPlacement.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationModifier.cpp
     ${CMAKE_SOURCE_DIR}/src/daemon/HNIrrigationTypes.cpp
)

SET( HNIRRIGATIOND_CLIENT_SRC
     ${CMAKE_SOURCE_DIR}/src/client/hnirrigationd_client.cpp
)

SET(CMAKE_BUILD_TYPE Debug)

FIND_PACKAGE( Poco REQUIRED Util Foundation Net NetSSL JSON )
FIND_PACKAGE( HNode2 REQUIRED )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/src/include )

ADD_EXECUTABLE( hnirrigationd ${HNIRRIGATIOND_SRC} )
TARGET_LINK_LIBRARIES( hnirrigationd PRIVATE
    ${Poco_Util_LIBRARY}
    ${Poco_Foundation_LIBRARY}
    ${Poco_Net_LIBRARY}
    ${Poco_NetSSL_LIBRARY}
    ${Poco_JSON_LIBRARY}
)
TARGET_LINK_LIBRARIES( hnirrigationd PRIVATE HNode2::common )

ADD_EXECUTABLE( hni_test_scheduler ${HNI_TEST_SCHEDULER_SRC} )
TARGET_LINK_LIBRARIES( hni_test_scheduler PRIVATE
    ${Poco_Util_LIBRARY}
    ${Poco_Foundation_LIBRARY}
    ${Poco_Net_LIBRARY}
    ${Poco_NetSSL_LIBRARY}
    ${Poco_JSON_LIBRARY}
)
TARGET_LINK_LIBRARIES( hni_test_scheduler PRIVATE HNode2::common )

ADD_EXECUTABLE( hnirrigationd_client ${HNIRRIGATIOND_CLIENT_SRC} )
TARGET_LINK_LIBRARIES( hnirrigationd_client PRIVATE
    ${Poco_Util_LIBRARY}
    ${Poco_Foundation_LIBRARY}
    ${Poco_Net_LIBRARY}
    ${Poco_NetSSL_LIBRARY}
    ${Poco_JSON_LIBRARY}
)
TARGET_LINK_LIBRARIES( hnirrigationd_client PRIVATE HNode2::common )

INSTALL( TARGETS hnirrigationd DESTINATION ${CMAKE_INSTALL_PREFIX}/sbin COMPONENT daemon )

INSTALL( TARGETS hnirrigationd_client DESTINATION ${CMAKE_INSTALL_PREFIX}/sbin COMPONENT client )

#CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/pkg/hnswitchd.service.in ${CMAKE_BINARY_DIR}/hnswitchd.service @ONLY )
#INSTALL( FILES ${CMAKE_BINARY_DIR}/hnswitchd.service DESTINATION /etc/systemd/system/ COMPONENT daemon )

SET( CPACK_GENERATOR "DEB" )

SET( CPACK_DEB_COMPONENT_INSTALL 1 )
SET( CPACK_COMPONENTS_ALL daemon client dev )

SET( CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION} )
SET( CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION} )
SET( CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION} )

SET( CPACK_PACKAGE_DESCRIPTION "An HNode2 device implementing an irrigation controller.  Utilizes the hnode2_switch_daemon to control the valves." )
SET( CPACK_PACKAGE_DESCRIPTION_SUMMARY "The hnode2 irrigation device." )
SET( CPACK_PACKAGE_CONTACT "Curtis Nottberg" )
SET( CPACK_PACKAGE_ARCHITECTURE DEB-DEFAULT )

SET( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
SET( CPACK_DEBIAN_PACKAGE_SECTION "Daemon" )
SET( CPACK_DEBIAN_PACKAGE_MAINTAINER "Curtis Nottberg" ) #required
SET( CPACK_DEBIAN_PACKAGE_VERSION ${DEBVERSION} )
SET( CPACK_DEBIAN_FILE_NAME DEB-DEFAULT )

INCLUDE(CPack)

