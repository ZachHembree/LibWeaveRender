param([string]$HeaderPath)

# Validate header file exists
if (-not (Test-Path $HeaderPath)) 
{
    Write-Error "Header file not found: $HeaderPath"
    exit 1
}

# Read the file content
try 
{
    $content = Get-Content -Path $HeaderPath -Raw -Encoding UTF8
    if (-not $content) 
    {
        Write-Error "File is empty or unreadable: $HeaderPath"
        exit 1
    }
} catch 
{
    Write-Error "Failed to read content from $HeaderPath : $_"
    exit 1
}

# Define the macro to match
$defStr = "#define\s+VERSION_BUILD\s+"
$regex = "$defStr(?<BuildVersion>\d+)\s*(\r?\n|$)"

# Match and increment
if ($content -match $regex) 
{
    $newBuild = [int]$matches["BuildVersion"] + 1 
    $newContent = $content -replace $regex, "#define VERSION_BUILD $newBuild`r`n"
    $newContent = $newContent -replace '\r?\n', "`r`n"
    
    try 
    {
        [System.IO.File]::WriteAllText($HeaderPath, $newContent, [System.Text.UTF8Encoding]::new($false))
        Write-Output "Build $newBuild ($HeaderPath)"
    } 
    catch 
    {
        Write-Error "Failed to write to $HeaderPath : $_"
        exit 1
    }
} 
else 
{
    Write-Error "Could not find VERSION_BUILD macro in $HeaderPath. Content:`n$content"
    Write-Error "Regex used: $regex"
    exit 1
}