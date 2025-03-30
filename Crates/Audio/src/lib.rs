pub struct AudioManager(kira::AudioManager::<kira::DefaultBackend>);

#[unsafe(no_mangle)]
pub extern "C" fn audio_manager_create() -> *mut AudioManager {
    match kira::AudioManager::<kira::DefaultBackend>::new(
        kira::AudioManagerSettings::default()
    ) {
        Ok(manager) => {
            let manager = Box::new(AudioManager(manager));
            Box::into_raw(manager)
        }
        Err(e) => {
            eprintln!("Failed to create audio manager: {e}");
            std::ptr::null_mut()
        }
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn audio_manager_destroy(ptr: *mut AudioManager) {
    drop(unsafe { Box::from_raw(ptr) });
}
