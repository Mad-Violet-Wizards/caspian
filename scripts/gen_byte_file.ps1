# Quick desc:
# Script is taking path to any file but pointing font/texture ones makes most sense.
# As an output it produces a .inc file for C++ with the byte array of the file to be processed by the engine.

# Usage:
# .\gen_byte_file.ps1 -InputPath "E:\Desktop\font.ttf" -OutputPath "E:\Desktop\font.inc"


param(
    [string]$InputPath,
    [string]$OutputPath,
    [string]$VariableName
)

$raw_bytes = [System.IO.File]::ReadAllBytes($InputPath)

$out_bytes_formated = "";
foreach($raw_byte in $raw_bytes)
{
    $out_bytes_formated += "0x" + $raw_byte.ToString("X2") + ", "
}
$out_bytes_formated = $out_bytes_formated.TrimEnd(", ".ToCharArray())

$output = [String]::Format("unsigned char {0}[] = {{ {1} }}; unsigned int {0}_size = {2};", $VariableName, $out_bytes_formated, $raw_bytes.Length)
$output | Out-File -FilePath $OutputPath

