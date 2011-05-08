<?php
// $Id: kml-placemark.tpl.php,v 1.1.2.1 2010/06/03 14:14:09 tmcw Exp $
?><Placemark>
  <name>
    <![CDATA[<?php print $name ?>]]>
  </name>
  <description>
    <![CDATA[<?php print $description ?>]]>
  </description>
  <?php if ($styleUrl): ?>
  <styleUrl><?php echo $styleUrl; ?></styleUrl>
  <?php endif; ?>
  <Point>
    <coordinates><?php print $coords ?></coordinates>
  </Point>
</Placemark>
