$c=[System.IO.File]::ReadAllText("c:\Users\Ayaz\Desktop\New hk\jni\src\Menu_Ui.hpp")
function C($p){return([regex]::Matches($c,$p)).Count}
function P($p){$t=0;foreach($m in [regex]::Matches($c,$p)){$n=$m.Groups[1].Value;if($n-eq''){$t+=1}else{$t+=[int]$n}};return $t}
$ok="OK";$bad="*** MISMATCH ***"
@(
  @("BeginChild/EndChild",(C 'BeginChild\('),(C 'EndChild\(\)')),
  @("Begin/End(win)",(C 'ImGui::Begin\('),(C 'ImGui::End\(\)')),
  @("PushColor/Pop",(C 'PushStyleColor\('),(P 'PopStyleColor\((\d*)')),
  @("PushVar/Pop",(C 'PushStyleVar\('),(P 'PopStyleVar\((\d*)')),
  @("BeginCard/EndCard",(C '\bBeginCard\('),(C '\bEndCard\(')),
  @("TabBar",(C 'BeginTabBar\('),(C 'EndTabBar\(\)')),
  @("TabItem",(C 'BeginTabItem\('),(C 'EndTabItem\(\)'))
)|ForEach-Object{"{0,-26} {1,2}/{2,-2} {3}"-f $_[0],$_[1],$_[2],(if($_[1]-ne$_[2]){$bad}else{$ok})}
