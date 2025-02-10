<# :
  @echo off
    powershell /nologo /noprofile /command ^
        "&{[ScriptBlock]::Create((cat """%~f0""") -join [Char[]]10).Invoke(@(&{$args}%*))}"
  exit /b
#>

function Add-Path-User($newPath) {
    $Path = $newPath + [IO.Path]::PathSeparator + [Environment]::GetEnvironmentVariable("PATH", "User") 
    [Environment]::SetEnvironmentVariable( "Path", $Path, "User" )
}

$bin="$HOME\Documents\getidmis"

Write-Host INSTALLING GETIDMIS -fo Green
Write-Host
Write-Host Creating a folder for getidmis: $bin -fo Green
New-Item -ItemType Directory -Path $bin -Force
Write-Host Adding this folder to your path -fo Green
####### Add-Path-User($bin)
####### $Path = [Environment]::GetEnvironmentVariable("PATH", "User") 
####### Write-Host Your path is now $Path -fo Green

$certPW = Read-Host "Enter your certificate password"
echo $certPW > "$bin\certpw.txt"
Write-Host Your certificate password is now stored in $bin\certpw.txt -fo Green

Write-Host Downloading the getidmis executables to $bin -fo Green
cd $bin
$file = "gi.exe"
$uri = "http://stagleys.co.uk/getidmis/$file"
curl -o $file $uri
$file = "curl.exe"
$uri = "http://stagleys.co.uk/getidmis/$file"
curl -o $file $uri
$file = "getidmis.bat"
$uri = "http://stagleys.co.uk/getidmis/$file"
curl -o $file $uri

Write-Host "You must now copy your certificate file into $bin" -fo Red
Write-Host "   and rename it cert.p12" -fo Red

$certPW = Read-Host "Press <ENTER> when done"





