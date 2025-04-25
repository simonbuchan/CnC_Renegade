#[repr(C)]
pub enum AudioSeekType {
    Start,
    Current,
    End,
}

#[repr(C)]
pub struct AudioFileApi {
    pub read: unsafe extern "C" fn(AudioFileHandle, *mut u8, usize) -> usize,
    pub seek: unsafe extern "C" fn(AudioFileHandle, i64, AudioSeekType) -> u64,
    pub close: unsafe extern "C" fn(AudioFileHandle),
}
unsafe impl Send for AudioFileApi {}
unsafe impl Sync for AudioFileApi {}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct AudioFileHandle {
    pub data: *const std::ffi::c_void,
}

unsafe impl Send for AudioFileHandle {}
unsafe impl Sync for AudioFileHandle {}

pub(crate) struct AudioFile {
    pub(crate) api: AudioFileApi,
    pub(crate) handle: AudioFileHandle,
}

impl symphonia::core::io::MediaSource for AudioFile {
    fn is_seekable(&self) -> bool {
        true
    }

    fn byte_len(&self) -> Option<u64> {
        let seek = self.api.seek;
        let result = unsafe { seek(self.handle, 0, AudioSeekType::End) };
        Some(result)
    }
}

impl Drop for AudioFile {
    fn drop(&mut self) {
        unsafe { (self.api.close)(self.handle) }
    }
}

impl std::io::Read for AudioFile {
    fn read(&mut self, buf: &mut [u8]) -> std::io::Result<usize> {
        let result = unsafe { (self.api.read)(self.handle, buf.as_mut_ptr(), buf.len()) };
        Ok(result)
    }
}

impl std::io::Seek for AudioFile {
    fn seek(&mut self, pos: std::io::SeekFrom) -> std::io::Result<u64> {
        let seek = self.api.seek;
        let result = match pos {
            std::io::SeekFrom::Start(offset) => unsafe {
                seek(self.handle, offset as i64, AudioSeekType::Start)
            },
            std::io::SeekFrom::Current(offset) => unsafe {
                seek(self.handle, offset, AudioSeekType::Current)
            },
            std::io::SeekFrom::End(offset) => unsafe {
                seek(self.handle, offset, AudioSeekType::End)
            },
        };
        Ok(result)
    }
}
