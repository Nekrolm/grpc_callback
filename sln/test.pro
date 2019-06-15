QT += core
TARGET = Test
TEMPLATE = app

ROOT_PATH = $$PWD/../

PROTOS_DIR = $${ROOT_PATH}/protos
GEN_PROTOS_DIR = $${ROOT_PATH}/src/protos


# make grpc protos
MAKE_PATH_CMD = "mkdir -p $${GEN_PROTOS_DIR}"
RET_VAL = $$system($${MAKE_PATH_CMD})
MAKE_PROTOS_CMD = "protoc -I $${PROTOS_DIR} --cpp_out=$${GEN_PROTOS_DIR} $${PROTOS_DIR}/*.proto"
MAKE_GRPC_CMD = "protoc -I $${PROTOS_DIR} --grpc_out=$${GEN_PROTOS_DIR} --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) $${PROTOS_DIR}/*.proto"

RET_VAL = $$system($${MAKE_GRPC_CMD})
RET_VAL = $$system($${MAKE_PROTOS_CMD})

SOURCES += $$files($${GEN_PROTOS_DIR}/*.cc)
HEADERS += $$files($${GEN_PROTOS_DIR}/*.h) 

LIBS += $$system("pkg-config --libs protobuf grpc++ grpc")
LIBS +=  -lgrpc++_reflection -ldl


CONFIG += C++14
QMAKE_CXXFLAGS += -std=c++14



INCLUDEPATH += $${ROOT_PATH}/src

SOURCES += $$files($${ROOT_PATH}/src/example/*.cpp)
SOURCES += $$files($${ROOT_PATH}/src/grpc/*.cpp)
HEADERS += $$files($${ROOT_PATH}/src/example/*.h)
HEADERS += $$files($${ROOT_PATH}/src/grpc/*.h)
