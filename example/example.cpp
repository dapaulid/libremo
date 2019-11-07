#include "remo.h"
#include "reader.h"
#include "writer.h"
#include "error.h"
#include "dynamic_call.h"

#include <stdint.h>
#include <iostream>

int hansli(uint32_t a1, char* a2, double a3, bool a4) {
    std::cout << "called hansli with " << a1 << " " << a2 << " " << a3 << " " << a4 << std::endl;
}

int main(int argc, char** argv)
{
    try {
        remo::Controller ctrl;

        uint8_t buffer [8] = {};
        //ctrl.call("Hansli", 0xAABBCCDD, "0000", 0.0, true, buffer, (remo::arraysize_t)1234);

        remo::Packet packet;
        remo::BinaryWriter writer(packet);
        writer.write_call("Hansli", 0xAABBCCDD, "gugus", 12.34, true, buffer, (remo::arraysize_t)1234);

        packet.log();

        remo::BinaryReader reader(packet);
        reader.read_call();

        std::cout << "Params: " << reader.get_args().size() << std::endl;
        remo::dynamic_call(&hansli, reader.get_args());

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