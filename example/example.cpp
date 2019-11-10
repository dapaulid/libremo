#include "remo.h"
#include "reader.h"
#include "writer.h"
#include "error.h"
#include "function.h"

#include <stdint.h>
#include <iostream>

uint32_t hansli(uint32_t a1, const char* a2, double a3, bool a4) {
    std::cout << "called hansli with " << a1 << " " << a2 << " " << a3 << " " << a4 << std::endl;
}

double fritzli(uint32_t* a1) {
    std::cout << "called hansli with " << *a1 << std::endl;
    (*a1)++;
    return 0.5;
}


std::any bar(uint32_t* a) {
    return a;
}

int main(int argc, char** argv)
{

    uint32_t foo = 100000;
    std::any a = bar(&foo);
    
    foo = 20000;

    std::cout << *std::any_cast<uint32_t*>(a) << std::endl;


    try {
        remo::Controller ctrl;

        remo::bound_function bound_fritzli("fritzli", &fritzli);
        std::cout << "bound function " << bound_fritzli.to_string() << std::endl;

        remo::Packet packet;
        remo::BinaryWriter writer(packet);
        uint32_t x = 5555;
        writer.write_call("fritzli", &x);

        std::cout << packet.to_hex() << std::endl;
        std::cout << packet << std::endl;

        remo::BinaryReader reader(packet);
        reader.read_call();

        std::cout << "Params: " << reader.get_args().size() << std::endl;
        remo::TypedValue result = bound_fritzli.call(reader.get_args());

        std::cout << packet << std::endl;

        remo::Packet reply;
        remo::BinaryWriter reply_writer(reply);

        remo::ArgList args = reader.get_args(); // TODO const? separate variable;
        reply_writer.write_result(result, args);

        std::cout << reply.to_hex() << std::endl;
        std::cout << reply << std::endl;

        /*
        BinaryWriter writer;
        writer.write_value((uint32_t)1234);
        writer.write_value((uint32_t)0xAABBCCDD);
        writer.write_value(0.0);
        writer.write_value(1.0);
        writer.write_value(-1.0);
        writer.write_value(1000.0);
        writer.write_value(0.1);
        writer.write_value("gugus");
        writer.write_value((char*)"foo");
        writer.write_value((const char*)"bar");
        */

        //uint8_t buffer [8] = {0x66};
        //writer.write_value(buffer);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}