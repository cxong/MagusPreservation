/* Copyright (C) 2014 Casale Media All Rights Reserved.
The information contained within this document is confidential, copyrighted and or trade secret. No part of this document may be reproduced or distributed in any form or by any means, in whole or in part, without the prior written permission of Casale Media.
*/
var CasaleR;if(!CasaleR){CasaleR=(new Date().getTime()%2592000)*256+Math.floor(Math.random()*256)+256;}
var CasaleP;if(!CasaleP){CasaleP=casaleFlashVer();}
function casaleCall(CasaleID,Args){var divID='CasaleDIV'+Math.random();document.write('<div id="'+divID+'" style="width:0; height:0;position:absolute;"></div>');var casaleU='';var amp='&amp;';var casaleIF=0;if(top==self){casaleU=encodeURIComponent(top.location.href);}else{casaleU=encodeURIComponent(document.referrer);casaleIF=1;}
casaleU=casaleU.substr(0,1024);var casaleL=0;var casaleT=0;var casaleW=window.screen.availWidth;var casaleH=window.screen.availHeight;var casaleObj=document.getElementById(divID);while(casaleObj!=null){casaleL+=casaleObj.offsetLeft;casaleT+=casaleObj.offsetTop;casaleObj=casaleObj.offsetParent;}
var casaleDomain;if(window.location.protocol=='https:'){casaleDomain='https://as-sec.casalemedia.com';}else{casaleDomain='http://as.casalemedia.com';}
var req='<script type="text/javascript" src="'+casaleDomain+'/j?s='+CasaleID+amp+'a='+Args.adUnits+amp+'id='+CasaleR+amp+'p='+CasaleP+amp+'v='+Args.version+amp+'inif='+casaleIF;if(Args.positionID){req+=amp+'pos='+Args.positionID;}
if(Args.pubDefault&&/\S/.test(Args.pubDefault)){req+=amp+'d=CasaleArgs.pubDefault';}
if(Args.align){req+=amp+'al='+Args.align;}
if(Args.floor&&/\S/.test(Args.floor)){req+=amp+'f='+encodeURIComponent(Args.floor);if(Args.floorCurrency&&/\S/.test(Args.floorCurrency)){req+=amp+'fc='+encodeURIComponent(Args.floorCurrency);}}
req+=amp+'l='+casaleL;req+=amp+'t='+casaleT;req+=amp+'w='+casaleW;req+=amp+'h='+casaleH;req+=amp+'z='+new Date().getTimezoneOffset();req+=amp+'u='+casaleU;if(!casaleIF&&document.referrer){req+=amp+'r='+(encodeURIComponent(document.referrer)).substr(0,512);}
req+='"><\/script>';document.write(req);}
function casaleFlashVer(){var version=0;var isIE=(navigator.appVersion.indexOf("MSIE")!=-1)?true:false;var isWin=(navigator.appVersion.toLowerCase().indexOf("win")!=-1)?true:false;var isOpera=(navigator.userAgent.indexOf("Opera")!=-1)?true:false;if(navigator.plugins&&navigator.plugins.length){if(navigator.plugins["Shockwave Flash 2.0"]||navigator.plugins["Shockwave Flash"]){var swVer=navigator.plugins["Shockwave Flash 2.0"]?" 2.0":"";var desc=navigator.plugins["Shockwave Flash"+swVer].description;var ary=desc.split(" ");version=ary[2].split(".")[0];}}else if(isIE&&isWin&&!isOpera){var tmp,verDesc;try{tmp=new ActiveXObject("ShockwaveFlash.ShockwaveFlash.7");version=tmp.getVariable("$version").split(" ")[1].split(",")[0];}catch(e){}
if(!version){try{tmp=new ActiveXObject("ShockwaveFlash.ShockwaveFlash.6");verDesc="WIN 6,0,21,0";tmp.AllowScriptAccess="always";version=tmp.getVariable("$version").split(" ")[1].split(",")[0];}catch(e){}}
if(!version){try{tmp=new ActiveXObject("ShockwaveFlash.ShockwaveFlash.3");version=tmp.getVariable("$version").split(" ")[1].split(",")[0];}catch(e){}}}
return version;}
function casaleIncl(src){var i=document.createElement("script");i.type="text/javascript";i.src=src;i.id="_cmjs";(document.getElementsByTagName("head")[0]||document.getElementsByTagName("body")[0]).appendChild(i);}
function casaleInclCSS(src){var i=document.createElement("link");i.type="text/css";i.href=src;i.rel='stylesheet';i.id="_cmcss";document.getElementsByTagName("head")[0].appendChild(i);}
casaleCall(CasaleArgs.casaleID,CasaleArgs);
