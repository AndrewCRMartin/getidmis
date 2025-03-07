<# :
  @echo off
    powershell /nologo /noprofile /command ^
        "&{[ScriptBlock]::Create((cat """%~f0""") -join [Char[]]10).Invoke(@(&{$args}%*))}"
  exit /b
#>

$bin="$HOME\Documents\getidmis"

Write-Host INSTALLING GETIDMIS -fo Green
Write-Host
Write-Host Creating a folder for getidmis: $bin -fo Green
New-Item -ItemType Directory -Path $bin -Force

$certPW = Read-Host "Enter your certificate password"
echo $certPW > "$bin\certpw.txt"
Write-Host Your certificate password is now stored in $bin\certpw.txt -fo Green

Write-Host Downloading the getidmis executables to $bin -fo Green
cd $bin
$file = "gi.exe"
$uri = "http://www.bioinf.org.uk/getidmis/windows/$file"
curl -o $file $uri
$file = "curl.exe"
$uri = "http://www.bioinf.org.uk/getidmis/windows/$file"
curl -o $file $uri
$file = "getidmis.bat"
$uri = "http://www.bioinf.org.uk/getidmis/windows/$file"
curl -o $file $uri

Write-Host "You must now copy your certificate file into $bin" -fo Red
Write-Host "   and rename it cert.p12" -fo Red

$certPW = Read-Host "Press <Enter> when done"
