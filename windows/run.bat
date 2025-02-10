<# :
  @echo off
    powershell /nologo /noprofile /command ^
        "&{[ScriptBlock]::Create((cat """%~f0""") -join [Char[]]10).Invoke(@(&{$args}%*))}"
  exit /b
#>

Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process

Write-Host RUNNING GETIDMIS -fo Green
Write-Host
$bin="$HOME\getidmis"

$exe="$bin\getidmis"
$param="-c $bin\cert.p12 -f $bin\certpw.txt"

Write-Host "Enter the INN request numbers you wish to download" -fo Green
Write-Host "   You may enter multiple numbers (space-separated) on one line" -fo Green
Write-Host "   or one per line (or a combination!)" -fo Green

$requests = ''
do
{
   $reqNum = Read-Host "Request numbers - or a blank line to exit"
   if($reqNum -ne '')
   {
      $requests = $requests + ' ' + $reqNum
   }
}  while($reqNum -ne '')

$param = $param + ' ' + $requests

Write-Host "Running command: $exe"

& $exe $param

Read-Host "Press return when done"
