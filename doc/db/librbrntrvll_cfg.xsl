<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">
	<xsl:param name="use.id.as.filename" select="'1'" />
	<xsl:param name="admon.graphics" select="'1'" />
	<xsl:param name="admon.graphics.path"></xsl:param>
	<xsl:param name="chunk.section.depth" select="1"></xsl:param>
	<xsl:param name="section.autolabel" select="1"></xsl:param>
	<xsl:template name="user.head.content">
		<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js"></script>
		<script type="text/javascript" src="scripts/jquery.snippet.min.js"></script>
		<script type="text/javascript" src="scripts/master.js"></script>
		<link rel="stylesheet" type="text/css" href="css/master.css" />
	</xsl:template>
</xsl:stylesheet>

