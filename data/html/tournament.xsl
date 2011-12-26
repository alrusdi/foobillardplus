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
      <div id="eightblock" style="display:none" class="tablediv">
      <xsl:for-each select="eightball">
      <!-- unix timestamp -->
      <xsl:sort select="gameid"/>
      <div>
      <xsl:element name="a">
        <xsl:attribute name="href">javascript:toggle('<xsl:value-of select="gameid"/>');</xsl:attribute>
        <xsl:value-of select="/start/tdate"/>: <b><xsl:value-of select="date"/></b> ,<xsl:value-of select="/start/twin"/>: <b><xsl:value-of select="allwin"/></b>
      </xsl:element>
        <!-- Begin Display none first round -->
        <xsl:element name="div">
          <xsl:attribute name="id"><xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play1_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 1:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play1_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g1">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none first round -->

        <!-- Begin Display none second round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play2_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 2:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play2_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g2">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none second round -->

        <!-- Begin Display none third round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play3_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 3:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play3_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g3">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none third round -->

        <!-- Begin Display none last round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play4_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 4:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play4_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g4">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none last round -->
        </xsl:element> <!-- End Display none eightblock -->
      </div>
      </xsl:for-each>
      </div>
      </p>
      <!-- end one gametype -->

      <!-- start one gametype--> 
      <!-- 9 Ball -->
      <h2><a href="#" onclick="toggle('nineblock');"><xsl:value-of select="ninetitle"/></a></h2>
      <p>
      <div id="nineblock" style="display:none" class="tablediv">
      <xsl:for-each select="nineball">
      <!-- unix timestamp -->
      <xsl:sort select="gameid"/>
      <div>
      <xsl:element name="a">
        <xsl:attribute name="href">javascript:toggle('<xsl:value-of select="gameid"/>');</xsl:attribute>
        <xsl:value-of select="/start/tdate"/>: <b><xsl:value-of select="date"/></b> ,<xsl:value-of select="/start/twin"/>: <b><xsl:value-of select="allwin"/></b>
      </xsl:element>
        <!-- Begin Display none first round -->
        <xsl:element name="div">
          <xsl:attribute name="id"><xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play1_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 1:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play1_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g1">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none first round -->

        <!-- Begin Display none second round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play2_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 2:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play2_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g2">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none second round -->

        <!-- Begin Display none third round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play3_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 3:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play3_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g3">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none third round -->

        <!-- Begin Display none last round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play4_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 4:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play4_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g4">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none last round -->
        </xsl:element> <!-- End Display none eightblock -->
      </div>
      </xsl:for-each>
      </div>
      </p>
      <!-- end one gametype -->

      <!-- start one gametype--> 
      <!-- Carom Ball -->
      <h2><a href="#" onclick="toggle('caromblock');"><xsl:value-of select="caromtitle"/></a></h2>
      <p>
      <div id="caromblock" style="display:none" class="tablediv">
      <xsl:for-each select="caromball">
      <!-- unix timestamp -->
      <xsl:sort select="gameid"/>
      <div>
      <xsl:element name="a">
        <xsl:attribute name="href">javascript:toggle('<xsl:value-of select="gameid"/>');</xsl:attribute>
        <xsl:value-of select="/start/tdate"/>: <b><xsl:value-of select="date"/></b> ,<xsl:value-of select="/start/twin"/>: <b><xsl:value-of select="allwin"/></b>
      </xsl:element>
        <!-- Begin Display none first round -->
        <xsl:element name="div">
          <xsl:attribute name="id"><xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play1_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 1:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play1_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g1">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none first round -->

        <!-- Begin Display none second round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play2_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 2:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play2_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g2">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none second round -->

        <!-- Begin Display none third round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play3_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 3:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play3_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g3">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none third round -->

        <!-- Begin Display none last round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play4_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 4:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play4_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g4">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none last round -->
        </xsl:element> <!-- End Display none eightblock -->
      </div>
      </xsl:for-each>
      </div>
      </p>
      <!-- end one gametype -->

      <!-- start one gametype--> 
      <!-- Snooker -->
      <h2><a href="#" onclick="toggle('snookerblock');"><xsl:value-of select="snookertitle"/></a></h2>
      <p>
      <div id="snookerblock" style="display:none" class="tablediv">
      <xsl:for-each select="snookerball">
      <!-- unix timestamp -->
      <xsl:sort select="gameid"/>
      <div>
      <xsl:element name="a">
        <xsl:attribute name="href">javascript:toggle('<xsl:value-of select="gameid"/>');</xsl:attribute>
        <xsl:value-of select="/start/tdate"/>: <b><xsl:value-of select="date"/></b> ,<xsl:value-of select="/start/twin"/>: <b><xsl:value-of select="allwin"/></b>
      </xsl:element>
        <!-- Begin Display none first round -->
        <xsl:element name="div">
          <xsl:attribute name="id"><xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play1_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 1:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play1_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g1">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none first round -->

        <!-- Begin Display none second round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play2_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 2:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play2_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g2">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none second round -->

        <!-- Begin Display none third round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play3_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 3:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play3_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g3">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none third round -->

        <!-- Begin Display none last round -->
        <div>
        <xsl:element name="a">
          <xsl:attribute name="href">javascript:toggle('play4_<xsl:value-of select="gameid"/>');</xsl:attribute>
          <xsl:value-of select="/start/tround"/> 4:
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="id">play4_<xsl:value-of select="gameid"/></xsl:attribute>
          <xsl:attribute name="style">display:none</xsl:attribute>
        <div class="headerdiv">
        <table>
          <tr>
            <th style="width:16%;"><xsl:value-of select="/start/column1"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column2"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column3"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column4"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column5"/></th>
            <th style="width:16%;"><xsl:value-of select="/start/column6"/></th>
            <th style="width:4%;"> </th>
          </tr>
        </table>
        </div>
        <div class="tablediv">
        <table>
          <xsl:for-each select="g4">
          <xsl:sort select="go"/>
          <tr>
            <td><xsl:value-of select="go"/></td>
            <td><xsl:value-of select="p1"/></td>
            <td><xsl:value-of select="p2"/></td>
            <td><xsl:value-of select="w"/></td>
            <td><xsl:value-of select="h"/></td>
            <td><xsl:value-of select="r"/></td>
          </tr>
          </xsl:for-each>
        </table>
        </div>
        </xsl:element> <!-- End Display none eightplay -->
        </div> <!-- End Display none last round -->
        </xsl:element> <!-- End Display none eightblock -->
      </div>
      </xsl:for-each>
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
