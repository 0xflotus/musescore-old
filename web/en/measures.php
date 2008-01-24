<?php
  $file="measures.php";
  require("header.html");
  ?>
<h4><a href="idx.php">MuseScore</a> -- <a href="manual.php">Documentation</a> -- <a href="reference.php">Index</a> -- Measure Operations</h4>
<h5>
Properties  </h5>
<table cellspacing="0" cellpadding="0">
  <tr>
    <td>
<b>Measure Duration</b>      <br/>

          Normally the nominal and actual duration of a measure is identical.
          An upbeat can have a different actual duration.
                <br/>
      <br/>
<b>Irregular</b>      <br/>

            An &quot;irregular&quot; measure will not be counted. Normally an upbeat
            is not counted as a regular measure.
                <br/>
      <br/>
<b>Repeat Count</b>      <br/>

            If the measure is the end of a
                  <a href="repeats.php">
repeat        </a>
 you can define how often the
            repeat should be played.

               </td>
    <td valign="top">
      <img src="../pic/measure.png"/>
      </td>
    </tr>
  </table>
<br/>
<?php require("trailer.html"); ?>
