cmake_minimum_required(VERSION 2.6)

BFG_INIT_PACKAGE(Model "0.3.3")

FILE(GLOB Model_Root_HEADERS *.h *.hh)
FILE(GLOB Model_Root_SOURCES *.cpp)
FILE(GLOB Model_Events_HEADERS Events/*.h)
FILE(GLOB Model_Events_SOURCES Events/*.cpp)
FILE(GLOB Model_Loader_HEADERS Loader/*.h)
FILE(GLOB Model_Loader_SOURCES Loader/*.cpp)
FILE(GLOB Model_Property_HEADERS Property/*.h)
FILE(GLOB Model_Property_SOURCES Property/*.cpp)
FILE(GLOB Model_Concepts_HEADERS Property/Concepts/*.h)
FILE(GLOB Model_Concepts_SOURCES Property/Concepts/*.cpp)

FILE(GLOB Model_SOURCES
	${Model_Root_SOURCES}
	${Model_Events_SOURCES}
	${Model_Loader_SOURCES}
	${Model_Property_SOURCES}
	${Model_Concepts_SOURCES}
	*.xml
)

FILE(GLOB Model_HEADERS
	${Model_Root_HEADERS}
	${Model_Events_HEADERS}
	${Model_Loader_HEADERS}
	${Model_Property_HEADERS}
	${Model_Concepts_HEADERS}
)

SOURCE_GROUP("." FILES ${Model_Root_HEADERS} ${Model_Root_SOURCES})
SOURCE_GROUP("Events" FILES ${Model_Events_HEADERS} ${Model_Events_SOURCES})
SOURCE_GROUP("Loader" FILES ${Model_Loader_HEADERS} ${Model_Loader_SOURCES})
SOURCE_GROUP("Model" FILES ${Model_Model_HEADERS} ${Model_Model_SOURCES})
SOURCE_GROUP("Property" FILES ${Model_Property_HEADERS} ${Model_Property_SOURCES})
SOURCE_GROUP("Property\\Concepts" FILES ${Model_Concepts_HEADERS} ${Model_Concepts_SOURCES})

INCLUDE_DIRECTORIES(${BoostLog_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OIS_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${MYGUI_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(..)
INCLUDE_DIRECTORIES(.)	# For subdirectories

ADD_DEFINITIONS(-DTIXML_USE_STL)

IF(WIN32)
	ADD_DEFINITIONS(-DMODEL_EXPORTS)
ENDIF(WIN32)

ADD_CUSTOM_TARGET(ModelEnums 
	bfgEnumGen --input ${CMAKE_CURRENT_SOURCE_DIR}/Enums.xml
	           --header ${CMAKE_CURRENT_SOURCE_DIR}/Enums.hh
	           --source ${CMAKE_CURRENT_SOURCE_DIR}/Enums.cpp
	DEPENDS bfgEnumGen
)

ADD_LIBRARY(bfgModel SHARED ${Model_HEADERS} ${Model_SOURCES})

SET_TARGET_PROPERTIES(bfgModel PROPERTIES OUTPUT_NAME_DEBUG "bfgModel_d")

TARGET_LINK_LIBRARIES(bfgModel
	${BoostLog_LIBRARIES}
	${Boost_LIBRARIES}
	TinyXML
	bfgBase
	bfgController
	bfgCore
	bfgEventCore
	bfgPhysics
	bfgView
)

ADD_DEPENDENCIES(bfgModel CoreEnums ModelEnums)

INSTALL(TARGETS bfgModel
	RUNTIME DESTINATION ${BFG_BIN_DIR}
	LIBRARY DESTINATION ${BFG_LIB_DIR}
	COMPONENT runtime
	ARCHIVE DESTINATION ${BFG_LIB_DIR}
	COMPONENT devel
)

INSTALL(FILES ${Model_Root_HEADERS}
	DESTINATION ${BFG_PACKAGE_INCLUDE_DIR}
	COMPONENT devel
)

INSTALL(FILES ${Model_Events_HEADERS}
	DESTINATION "${BFG_PACKAGE_INCLUDE_DIR}/Events"
	COMPONENT devel
)

INSTALL(FILES ${Model_Loader_HEADERS}
	DESTINATION "${BFG_PACKAGE_INCLUDE_DIR}/Loader"
	COMPONENT devel
)

INSTALL(FILES ${Model_Property_HEADERS}
	DESTINATION "${BFG_PACKAGE_INCLUDE_DIR}/Property"
	COMPONENT devel
)

INSTALL(FILES ${Model_Concepts_HEADERS}
	DESTINATION "${BFG_PACKAGE_INCLUDE_DIR}/Property/Concepts"
	COMPONENT devel
)

