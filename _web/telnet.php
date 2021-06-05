<?php include 'incs/inc_spage.php'; ?>

<table valign="top" align="center" width="900" class="prompt">
<tr><td align="center">	

The telnet instance runs in another location and may be not available.

<div id="fTelnetContainer" class="fTelnetContainer" style="background-image:images/crt.png;"></div>
<script>document.write('<script src="//embed-v2.ftelnet.ca/js/ftelnet-loader.norip.xfer.js?v=' + (new Date()).getTime() + '"><\/script>');</script>
<script>
    var Options = new fTelnetOptions();
    Options.BareLFtoCRLF = true;
    Options.BitsPerSecond = 57600;
    Options.ConnectionType = 'telnet';
    Options.Emulation = 'ansi-bbs';
    Options.Enter = '\r\n';
    Options.Font = 'CP437';
    Options.ForceWss = false;
    Options.Hostname = 'baffa.zapto.org';
    Options.LocalEcho = false;
    Options.NegotiateLocalEcho = true;
    Options.Port = 20248;
    Options.ProxyHostname = 'proxy-us-nj.ftelnet.ca';
    Options.ProxyPort = 80;
    Options.ProxyPortSecure = 443;
    Options.ScreenColumns = 80;
    Options.ScreenRows = 25;
    Options.SendLocation = true;
    var fTelnet = new fTelnetClient('fTelnetContainer', Options);
    fTelnet.Connect();
</script>
<br />
</td></tr>
</table>

<?php include 'incs/inc_epage.php'; ?>