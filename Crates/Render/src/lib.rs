use windows::Win32::UI::WindowsAndMessaging::{MB_OK, MessageBoxA};
use windows::core::PCSTR;

#[unsafe(no_mangle)]
pub extern "C" fn render_message(message_ptr: *const u8, message_len: usize) {
    let message = unsafe { std::slice::from_raw_parts(message_ptr, message_len) };
    let message = String::from_utf8_lossy(message);
    let message = message.to_string();
    let message = std::ffi::CString::new(message).unwrap();
    unsafe {
        MessageBoxA(
            None,
            PCSTR(message.as_ptr().cast()),
            PCSTR("Render Crate\0".as_ptr()),
            MB_OK,
        )
    };
}
