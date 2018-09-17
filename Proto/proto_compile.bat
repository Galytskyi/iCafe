echo Compile Protobuf file order.proto

protoc -I=. --cpp_out=. order.proto

pause