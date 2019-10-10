setPreference -pref UserLevel:NOVICE
setPreference -pref MessageLevel:DETAILED
setPreference -pref ConcurrentMode:FALSE
setPreference -pref UseHighz:FALSE
setPreference -pref ConfigOnFailure:STOP
setPreference -pref StartupCLock:AUTO_CORRECTION
setPreference -pref AutoSignature:FALSE
setPreference -pref KeepSVF:FALSE
setPreference -pref svfUseTime:FALSE

setMode -bs
setCable -port svf -file point4_lambert_pld_project_revc.svf
addDevice -position 1 -file point4_lambert_pld_project_revc.jed
erase -p 1
program -p 1
quit

