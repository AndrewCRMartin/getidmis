<# :
  @echo off
    powershell /nologo /noprofile /command ^
        "&{[ScriptBlock]::Create((cat """%~f0""") -join [Char[]]10).Invoke(@(&{$args}%*))}"
  exit /b
#>

$Path = ""
[Environment]::SetEnvironmentVariable( "Path", $Path, "User" )

