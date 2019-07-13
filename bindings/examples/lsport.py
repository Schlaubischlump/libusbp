from pyusbp import list_connected_devices, SerialPort, USBError

def serial_number_or_default(device, default="-"):
    try:
        return device.serial_number()
    except USBError as e:
        return False

def try_print_port_name(device, interface_number, composite):
    try:
        port = SerialPort(device, interface_number, composite)
        port_name = port.get_name()
    except USBError as e:
        return False
    print(port_name)
    return True


devices = list_connected_devices()
for dev in devices:
    vendor_id = dev.get_vendor_id();
    product_id = dev.get_product_id();
    serial_number = serial_number_or_default(device);

    for i in range(16):
        success = try_print_port_name(dev, i, True)

    if not success:
        try_print_port_name(dev, 0, False)
