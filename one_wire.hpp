enum OneWireState { READ, WRITE };


struct OneWire {
    int data_pin;
    OneWireState state;
};


void initialize_one_wire(OneWire& one_wire, int data_pin);

void set_state(OneWire& one_wire, OneWireState state);

OneWireState get_state(OneWire& one_wire);

void set_pin_value(OneWire& one_wire, bool value);

void write_bit(OneWire& one_wire, bool value);

bool read_bit(OneWire& one_wire);

bool wait_for_bit(OneWire& one_wire, bool bit, int max_time_us);
