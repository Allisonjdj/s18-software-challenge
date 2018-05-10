package serial

import (
    "github.com/jacobsa/go-serial/serial"
    "io"
)

var configInstance config

type config struct {
    Options serial.OpenOptions
    Port io.ReadWriteCloser
}

func Setup(baudrate uint, port string) (error) {
    options := serial.OpenOptions{
        PortName: port,
        BaudRate: baudrate,
        DataBits: 8,
        StopBits: 1,
        MinimumReadSize: 1,
    }

    configInstance = config{Options:options}

    // Open the port.
    if openPort, err := serial.Open(configInstance.Options); err != nil {
        return err
    } else {
        configInstance.Port = openPort
    }

    return nil
}

type WEPacket struct {
    Start uint8
    Id uint8
    End uint8
}

func Read() (*WEPacket) {
    buffer := make([]byte, 3)
    configInstance.Port.Read(buffer)

    if buffer[0] == 0x56 && buffer[2] == 0x23 {
        return &WEPacket{Start: buffer[0], Id: buffer[1], End:buffer[2]}
    }

    return nil
}

func Close() (error) {
    return configInstance.Port.Close();
}
