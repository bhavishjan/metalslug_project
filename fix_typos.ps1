$content = Get-Content "Level.h"
$content = $content.Replace('BazookaSoldier', 'BazookaSoldier')
Set-Content "Level.h" $content
