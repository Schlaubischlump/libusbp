#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>

#include "../include/libusbp.hpp"


namespace py = pybind11;
namespace usb = libusbp;

PYBIND11_MODULE(pyusbp, m) {
    m.doc() = "Python bindings for the libusbp library.";

    m.def("list_connected_devices", &usb::list_connected_devices);
    m.def("find_device_with_vid_pid", &usb::find_device_with_vid_pid);

    py::register_exception<usb::error>(m, "USBError");

    /*py::class_<usb::error> err(m, "USBError");
    err.def("message", &usb::error::message);
    err.def("what", &usb::error::what);*/

    py::enum_<libusbp_error_code>(m, "USBErrorCode")
    .value("MEMORY", LIBUSBP_ERROR_MEMORY)
    .value("NOT_READY", LIBUSBP_ERROR_NOT_READY)
    .value("ACCESS_DENIED", LIBUSBP_ERROR_ACCESS_DENIED)
    .value("NO_SERIAL_NUMBER", LIBUSBP_ERROR_NO_SERIAL_NUMBER)
    .value("TIMEOUT", LIBUSBP_ERROR_TIMEOUT)
    .value("DEVICE_DISCONNECTED", LIBUSBP_ERROR_DEVICE_DISCONNECTED)
    .value("STALL", LIBUSBP_ERROR_STALL)
    .value("CANCELLED", LIBUSBP_ERROR_CANCELLED)
    .export_values();

    py::class_<usb::device> dev(m, "USBDevice");
    dev.def("get_vendor_id", &usb::device::get_vendor_id);
    dev.def("get_product_id", &usb::device::get_product_id);
    dev.def("get_revision", &usb::device::get_revision);
    dev.def("get_serial_number", &usb::device::get_serial_number);
    dev.def("get_os_id", &usb::device::get_os_id);

    py::class_<usb::generic_handle> handle(m, "GenericHandler");
    handle.def(py::init<usb::generic_interface>());
    handle.def("close", &usb::generic_handle::close);
    handle.def("open_async_in_pipe", &usb::generic_handle::open_async_in_pipe);
    handle.def("set_timeout", &usb::generic_handle::set_timeout);
    handle.def("control_transfer", &usb::generic_handle::control_transfer);
    handle.def("write_pipe", &usb::generic_handle::write_pipe);
    handle.def("read_pipe", &usb::generic_handle::read_pipe);
#ifdef _WIN32
    handle.def("get_winusb_handle", &usb::generic_handle::get_winusb_handle);
#endif
#ifdef __linux__
    handle.def("get_fd", &usb::generic_handle::get_fd);
#endif
#ifdef __APPLE__
    handle.def("get_cf_plug_in", &usb::generic_handle::get_cf_plug_in);
#endif

    py::class_<usb::generic_interface> interface(m, "GenericInterface");
    interface.def(py::init<usb::device&, uint8_t, bool>(), py::arg("device"), py::arg("interface_number")=0, py::arg("composite")=false);

    interface.def("get_os_id", &usb::generic_interface::get_os_id);
    interface.def("get_os_filename", &usb::generic_interface::get_os_filename);

    py::class_<usb::serial_port> serialPort(m, "SerialPort");
    serialPort.def(py::init<usb::device&, uint8_t, bool>(), py::arg("device"), py::arg("interface_number")=0, py::arg("composite")=false);
    serialPort.def("get_name", &usb::serial_port::get_name);

    py::class_<usb::async_in_pipe> inPipe(m, "AsyncInPipe");
    inPipe.def("allocate_transfers", &usb::async_in_pipe::allocate_transfers);
    inPipe.def("start_endless_transfers", &usb::async_in_pipe::start_endless_transfers);
    inPipe.def("handle_events", &usb::async_in_pipe::handle_events);
    inPipe.def("has_pending_transfers", &usb::async_in_pipe::has_pending_transfers);
    inPipe.def("handle_finished_transfer", &usb::async_in_pipe::handle_finished_transfer);
    inPipe.def("cancel_transfers", &usb::async_in_pipe::cancel_transfers);
}
