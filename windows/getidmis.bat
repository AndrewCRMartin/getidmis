<# :
  @echo off
    powershell /nologo /noprofile /command ^
        "&{[ScriptBlock]::Create((cat """%~f0""") -join [Char[]]10).Invoke(@(&{$args}%*))}"
  exit /b
#>

Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process

$bin="$HOME\Documents\getidmis"

Write-Host RUNNING GETIDMIS -fo Green
Write-Host

$exe="$bin\gi"
$param="-c $bin\cert.p12 -f $bin\certpw.txt -x $bin\curl.exe"

Write-Host "Enter the INN request numbers you wish to download" -fo Green
Write-Host "   You may enter multiple numbers (space-separated) on one line" -fo Green
Write-Host "   or one per line (or a combination!)" -fo Green

$requests = ''
do
{
   $reqNum = Read-Host "Request numbers - or a blank line to finish"
   if($reqNum -ne '')
   {
      if($requests -eq '')
      {
          $requests = $reqNum
      }
      else
      {
          $requests = $requests + " $reqNum"
      }
   }
}  while($reqNum -ne '')

$cmd = "& $exe $param $requests"
Write-Host "Running command: $cmd"
Invoke-Expression $cmd
Write-Host "Downloading complete!" -fo Red

Read-Host "Press <Enter> when done"
