use serialport::prelude::*;
use std::io::prelude::*;
use std::thread;
use std::time;

// MCU Information
const COM_PORT: &str = "COM5";
/// Built-in LED
const LED_BUILTIN: u8 = 13;
/// Prescaler used on TIMER1
const PRESCALER: u32 = 8;
/// MCU clock frequency
const FREQ: u32 = 16000000;
/// Shortest delay on TIMER1 with given prescaler
const TIME_PER_CYCLE: f32 = (PRESCALER as f32) / (FREQ as f32);
/// Longest delay on TIMER1 with given prescaler and delay size
const MAX_TIME: f32 = (TIME_PER_CYCLE as f32) * (0xFFFFFFFFu32 as f32);

// CONSTANT COMMAND PACKETS
const REQUEST_SCHEDULE_SIZE: [u8; 8] = [0x01, 0x01, 0, 0, 0, 0, 0, 0];

fn main() {
    // Define serial port settings, default with a baudrate of 115200 and timeout of 1 sec
    let mut port_settings: SerialPortSettings = Default::default();
    port_settings.baud_rate = 115200;
    port_settings.timeout = time::Duration::from_secs(1);

    match serialport::open_with_settings(&COM_PORT, &port_settings) {
        Ok(mut port) => {
            // Pull DTR low to prepare MCU
            port.write_data_terminal_ready(false);
            // 1 second wait
            thread::sleep(time::Duration::from_millis(1000));
            loop {
                mcu_control_loop(&mut port);
            }
        }
        Err(e) => {
            eprintln!("Failed to open {} ( {} )", COM_PORT, e);
            ::std::process::exit(1);
        }
    }
}

/// Runs communications for given serial port
///
/// ## Arguments
///
/// * `port` - Serial port object
///
/// ## Example
/// ```
/// use serialport::prelude::*;
/// const COM_PORT: &str = "COM5";
///
/// fn main() {
///     let mut port_settings: SerialPortSettings = Default::default();
///     port_settings.baud_rate = 115200;
///
///     match serialport::open_with_settings(&COM_PORT, &port_settings) {
///         Ok(mut port) => loop {
///             mcu_communication_loop(&port);
///         },
///         Err(e) => {
///             eprintln!("Failed to open {} \n Err: {}", COM_PORT, e);
///             ::std::process::exit(1);
///         }
///     }
/// }
/// ```
fn mcu_control_loop(port: &mut Box<dyn SerialPort>) {
    // Create a mutable array of 8 unsigned bytes to read into
    // println!("let mut read_buf: [u8; 8] = Default::default();");
    let mut read_buf: [u8; 8] = Default::default();

    // Here I pass a reference to the constant REQUEST_SCHEDULE_SIZE
    // and pass read_buf as a mutable reference.
    send_bytes(port, &REQUEST_SCHEDULE_SIZE, &mut read_buf);

    // Create byte array from read buffer
    let mem_left: [u8; 4] = [0, 0, read_buf[1], read_buf[2]];
    let mem_left = u32::from_be_bytes(mem_left);

    // Compute delay and assemble digital output write to pin 13 of MCU
    let delay = mcu_delay_as_bytes(time::Duration::from_millis(500));
    let mut req: [u8; 8] = [0x02, LED_BUILTIN, 1, delay.3, delay.2, delay.1, delay.0, 0];
    for _ in 1..mem_left {
        send_bytes(port, &req, &mut read_buf);
        req[2] = if req[2] == 1 { 0 } else { 1 };
    }

    loop {}
}

/// Send bytes to device and compare response with checksum
///
/// `data` and `resp` must be a fixed size of 8 bytes
///
/// ## Arguments
/// * `port` - Serial port object
/// * `data` - Bytes to send to the device
/// * `resp` - Buffer where the response is stored
fn send_bytes(port: &mut Box<dyn SerialPort>, data: &[u8; 8], resp: &mut [u8; 8]) {
    // Data and response buffers must be 8 bytes
    if data.len() < 8 {
        eprintln!("Err: Data! < 8 bytes");
        return;
    }

    if resp.len() < 8 {
        eprintln!("Err: Buffer! < 8 bytes");
        return;
    }
    let checksum = compute_checksum(&data);
    let mut checksum_match = false;
    while checksum_match == false {
        // Write data to the port
        // Keep trying for a checksum match
        // println!("OUT[{}] -> {:?}", COM_PORT, data);
        port.write(data);
        port.read(resp);
        // println!("IN[{}] <- {:?}", COM_PORT, resp);
        checksum_match = resp[0] == checksum;

        if checksum_match == false {
            eprintln!("Err: Checksum mismatch ( {} != {} )", checksum, resp[0]);
            ::std::process::exit(1);
        }
    }
}

/// Return checksum for given bytes
///
/// ## Arguments
/// `data_slice` - Bytes to compute checksum for
fn compute_checksum(data_slice: &[u8; 8]) -> u8 {
    let mut sum: u32 = 0;

    // Cast the bytes to 32-bit integers and sum
    for byte in data_slice.iter() {
        sum += *byte as u32;
    }

    // Return the last byte of the sum
    (sum & 0xFF) as u8
}

/// Returns delay counter for TIMER1 as an tuple of 4 bytes (big endian)
fn mcu_delay_as_bytes(time: time::Duration) -> (u8, u8, u8, u8) {
    let count = if time.as_secs_f32() > MAX_TIME {
        0xFFFFFFFFu32
    } else {
        (time.as_secs_f32() / TIME_PER_CYCLE) as u32
    };
    let bit_3 = ((count & 0xFF000000) >> 24) as u8;
    let bit_2 = ((count & 0xFF0000) >> 16) as u8;
    let bit_1 = ((count & 0xFF00) >> 8) as u8;
    let bit_0 = ((count & 0xFF) >> 0) as u8;
    (bit_0, bit_1, bit_2, bit_3)
}
