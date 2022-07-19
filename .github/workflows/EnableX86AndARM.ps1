$texts = @(
    " '`$(Platform)' is not a valid architecture for Kernel mode drivers or UMDF drivers"
    " '`$(TargetVersion)' is not a supported OS Version"
)

$path  = Resolve-Path "C:\Program Files (x86)\Windows Kits\10\build\*\WindowsDriver.common.targets"
[XML] $targets = Get-Content $path

Write-Host ("[*] Loaded targets ""{0}"" into ""{1}""" -f $path, $targets)

[System.Xml.XmlNamespaceManager] $nsmgr = $targets.NameTable
$nsmgr.AddNamespace('ns', 'http://schemas.microsoft.com/developer/msbuild/2003')

foreach ( $text in $texts )
{
    $XPath = [string]::Format('//ns:Error[@Text="{0}"]', $text)
    Write-Host ("[+] XPath is ""{0}""" -f $XPath)

    $node  = $targets.SelectSingleNode($XPath, $nsmgr)
    if ($node)
    {
        Write-Host ("[-] Removing node ""{0}""" -f $node.Text)
        $node.ParentNode.RemoveChild($node);
    }
}

$targets.Save($path)
