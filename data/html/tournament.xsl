<?xml version="1.0" encoding="utf-8"?>
<!-- Edited by FooBillard++ -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/start">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="pragma" content="no-cache" />
<meta http-equiv="cache-control" content="no-cache" />
<meta http-equiv="expires" content="0" />
<title><xsl:value-of select="title"/></title>
<style type="text/css" media="all">
@import url("styles.css");
</style>
<!--[if IE 5]>
<style type="text/css"> 
#outerWrapper #contentWrapper #leftColumn1 {
  width: 200px;
}
</style>
<![endif]-->
<!--[if IE]>
<style type="text/css"> 
#outerWrapper #contentWrapper #content {
  zoom: 1;
}
</style>
<![endif]-->
<script language="JavaScript">
function toggle(toggleMe) {
  document.getElementById(toggleMe).style.visibility = "visible";
  if(document.getElementById(toggleMe).style.display == "none" ) {
    document.getElementById(toggleMe).style.display = "";
  }
  else {
    document.getElementById(toggleMe).style.display = "none";
  }
}
</script>
</head>
<body>
<div id="header">
  <div id="logo"><img src="images/logo.jpg" alt="Logo" width="299" height="54" /></div>
  <div id="links"><a href="mailto:foobillardplus@go4more.de">Contact Us</a></div>
  <div class="clearFloat"></div>
</div>
<div id="outerWrapper">
  <div id="gradient">
    <div id="feature"></div>
    <div id="contentWrapper">
      <div id="content">
      <!-- start one gametype--> 
      <!-- 8 Ball -->
      <h1><xsl:value-of select="title"/></h1><br />
      <h2><a href="#" onclick="toggle('eightblock');"><xsl:value-of select="eighttitle"/></a></h2>
      <p>
      <div id="eightblock" style="display:none">
      <div><a href="#" onclick="toggle('eightplayone');">Tournament date: <xsl:value-of select="playdate"/> Tournament Winner: <xsl:value-of select="overallwinner"/></a>
        <!-- Begin Display none first round -->
        <div id="eightplayone" style="display:none">
        <div><a href="#" onclick="toggle('eightplay1');">Winner round 1: <xsl:value-of select="roundonewinner"/></a>
        <div id="eightplay1" style="display:none">
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="column1"/></th>
            <th style="width:16%;"><xsl:value-of select="column2"/></th>
            <th style="width:16%;"><xsl:value-of select="column3"/></th>
            <th style="width:16%;"><xsl:value-of select="column4"/></th>
            <th style="width:16%;"><xsl:value-of select="column5"/></th>
            <th style="width:16%;"><xsl:value-of select="column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="eightball">
          <xsl:sort select="date"/>
          <tr>
            <td><xsl:value-of select="date"/></td>
            <td><xsl:value-of select="player1"/></td>
            <td><xsl:value-of select="player2"/></td>
            <td><xsl:value-of select="winner"/></td>
            <td><xsl:value-of select="hits"/></td>
            <td><xsl:value-of select="rounds"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </div> <!-- End Display none eightplay -->
        </div> <!-- End Display none first round -->

        <!-- Begin Display none second round -->
        <div><a href="#" onclick="toggle('eightplay2');">Winner round 2: <xsl:value-of select="roundtwowinner"/></a>
        <div id="eightplay2" style="display:none">
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="column1"/></th>
            <th style="width:16%;"><xsl:value-of select="column2"/></th>
            <th style="width:16%;"><xsl:value-of select="column3"/></th>
            <th style="width:16%;"><xsl:value-of select="column4"/></th>
            <th style="width:16%;"><xsl:value-of select="column5"/></th>
            <th style="width:16%;"><xsl:value-of select="column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="eightball">
          <xsl:sort select="date"/>
          <tr>
            <td><xsl:value-of select="date"/></td>
            <td><xsl:value-of select="player1"/></td>
            <td><xsl:value-of select="player2"/></td>
            <td><xsl:value-of select="winner"/></td>
            <td><xsl:value-of select="hits"/></td>
            <td><xsl:value-of select="rounds"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </div> <!-- End Display none eightplay -->
        </div> <!-- End Display none second round -->

        <!-- Begin Display none third round -->
        <div><a href="#" onclick="toggle('eightplay3');">Winner round 3: <xsl:value-of select="roundthreewinner"/></a>
        <div id="eightplay3" style="display:none">
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="column1"/></th>
            <th style="width:16%;"><xsl:value-of select="column2"/></th>
            <th style="width:16%;"><xsl:value-of select="column3"/></th>
            <th style="width:16%;"><xsl:value-of select="column4"/></th>
            <th style="width:16%;"><xsl:value-of select="column5"/></th>
            <th style="width:16%;"><xsl:value-of select="column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="eightball">
          <xsl:sort select="date"/>
          <tr>
            <td><xsl:value-of select="date"/></td>
            <td><xsl:value-of select="player1"/></td>
            <td><xsl:value-of select="player2"/></td>
            <td><xsl:value-of select="winner"/></td>
            <td><xsl:value-of select="hits"/></td>
            <td><xsl:value-of select="rounds"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </div> <!-- End Display none eightplay -->
        </div> <!-- End Display none third round -->

        <!-- Begin Display none last round -->
        <div><a href="#" onclick="toggle('eightplaylast');">Winner round 4: <xsl:value-of select="roundfourwinner"/></a>
        <div id="eightplaylast" style="display:none">
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="column1"/></th>
            <th style="width:16%;"><xsl:value-of select="column2"/></th>
            <th style="width:16%;"><xsl:value-of select="column3"/></th>
            <th style="width:16%;"><xsl:value-of select="column4"/></th>
            <th style="width:16%;"><xsl:value-of select="column5"/></th>
            <th style="width:16%;"><xsl:value-of select="column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="eightball">
          <xsl:sort select="date"/>
          <tr>
            <td><xsl:value-of select="date"/></td>
            <td><xsl:value-of select="player1"/></td>
            <td><xsl:value-of select="player2"/></td>
            <td><xsl:value-of select="winner"/></td>
            <td><xsl:value-of select="hits"/></td>
            <td><xsl:value-of select="rounds"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </div> <!-- End Display none eightplay -->
        </div> <!-- End Display none last round -->
      </div> <!-- End Display none eightblock -->
      </div>
      </div>
      </p>
      <!-- end one gametype -->
      </div>
      <br class="clearFloat" />
    </div>
    <img src="http://www.justdreamweaver.com/templates/link/spacer.gif" width="1" height="1" />
    <div id="footer"><a href="http://foobillardplus.sourceforge.net">Copyright (c) 2011 Foobillard++ Site (the project itself is licenced under the GPL)</a></div>
    <!--The following code must be left in place and unaltered for free usage of this theme. If you wish to remove the links, visit http://www.justdreamweaver.com/dreamweaver-template.html and purchase a template license.-->
  </div>
</div><div id="credit"><a href="http://www.justdreamweaver.com/dreamweaver-templates.html">Dreamweaver templates</a> by <a href="http://www.justdreamweaver.com">JustDreamweaver.com</a></div>
</body>
</html>
</xsl:template>
</xsl:stylesheet>
