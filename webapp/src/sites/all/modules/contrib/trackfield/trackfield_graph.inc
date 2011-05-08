<?php

/* Where we will store our cached graphs */
DEFINE('TRACKFIELD_GRAPH_CACHE_PATH', 'trackfield_cache');

/* So JpGraph knows where to find TTF */
DEFINE('TTF_DIR', variable_get('trackfield_graph_ttf_path', ''));

function _trackfield_graph_get_graphdata(&$graphargs, $settypes_in = FALSE) {
  $dataset = array();
  
  /*
   * Load the nodes so we can return a not found if there are none.
   */
  /* TODO: when same node, multiple sets are given only load the node once */
  for ($lp = 0; $lp < count($graphargs['items']); $lp++) {
    $oneset = array();
    $item = $graphargs['items'][$lp];
    /* Use given sets if override used */
    if (is_array($settypes_in)) {
      $settypes = $settypes_in;
    } else {
      /* Get x/y sets specified from args */
      $settypes = array();
      /* TODO: handle multiple axis */
      if (array_key_exists('xset', $item)) {
        $settypes[] = $item['xset'];
        $xset =  $item['xset'];
      }
      if (array_key_exists('yset', $item)) {
        $settypes[] = $item['yset'];
        $yset = $item['yset'];
      }
    }
    /* Get the node and data sets required */
    $node = node_load($item['nid'], $item['vid']);
    /* Shouldn't be possible to not have any settypes, but check anyway */
    if (!$node->nid || !isset($settypes)) {
      /* Remove from list anything we didn't find. */
      unset($graphargs['items'][$lp]);
      continue;
    }
    $field = $item['field'];
    $delta = $item['delta'];
    
    /* For the modified header */
    $modified = max($modified, $node->revision_timestamp);

    $fset = $node->$field;
    if (!is_array($fset)) {
      /* Remove from list anything we didn't find. */
      unset($graphargs['items'][$lp]);
      continue;
    }
    $oneset['title'] = $node->title;
    /* Load the data lists from separate table */

    $sql = <<<EOF
SELECT settype, setdata
FROM {trackfield_datasets} td
WHERE td.nid = %d
AND td.vid = %d
AND td.field_name = '%s'
AND td.delta = %d
AND settype = '%s'
EOF;

    foreach ($settypes as $settype) {
      $result = db_query($sql, $node->nid, $node->vid, $field, $delta, $settype);
      if ($row = db_fetch_object($result)) {
        $oneset[$row->settype] = split(',', $row->setdata);
      }
    }
    $dataset[] = $oneset;
  }
  if (count($dataset) == 0) {
    drupal_not_found();
    exit;
  }

  $bounds = array();
  $normal_type = $graphargs['normal_type'];
  if (count($dataset) > 1 || $normal_type != TRACKFIELD_NORM_NOTHING) {
    /*
     * More than one set, get combined bounds, and normalise them
     * if we are supposed to do that at the same time.
     */
    $x_min = 0;
    $x_max = -9999999;
    $y_min = 9999999;
    $y_max = -9999999;
    foreach ($dataset as $setno => &$oneset) {
      $xset = $graphargs['items'][$setno]['xset'];
      $yset = $graphargs['items'][$setno]['yset'];
      
      $points = count($oneset[$xset]);
      if ($normal_type != TRACKFIELD_NORM_NOTHING) {
        /* See if we have to reverse this set */
        $reverse = FALSE;
        switch($normal_type) {
          case TRACKFIELD_NORM_FORWARD:
            $reverse = FALSE;
            break;
          case TRACKFIELD_NORM_REVERSE:
            $reverse = TRUE;
            break;
          case TRACKFIELD_NORM_AS_CLIMB:
            if ($yset == 'altitude' && $oneset[$yset][0] > $oneset[$yset][$points - 1]) {
              $reverse = TRUE;
            } else {
              $reverse = FALSE;
            }
            break;
          case TRACKFIELD_NORM_AS_DESCENT:
            if ($yset == 'altitude' && $oneset[$yset][$points - 1] > $oneset[$yset][0]) {
              $reverse = TRUE;
            } else {
              $reverse = FALSE;
            }
            break;
        }
        /* Reverse if necessary */
        if ($reverse) {
          $oneset[$yset] = array_reverse($oneset[$yset]);
//          if (array_key_exists('latitude', $oneset)) { $oneset['latitude'] = array_reverse($oneset['latitude']); }
//          if (array_key_exists('longitude', $oneset)) { $oneset['longitude'] = array_reverse($oneset['longitude']); }
          /* X axis is a bit different */
          $tmp_arr = array();
          $one_dst_max = $oneset[$xset][$points - 1];
          for ($lp = $points - 1; $lp >= 0; $lp--) {
            $tmp_arr[] = $one_dst_max - $oneset[$xset][$lp];
          }
          $oneset[$xset] = $tmp_arr;
        }
      }
      /* Now get altitude bounds */

      /* Now get data bounds */
      /* Bounds for Y */
      $base_y = min($oneset[$yset]);
      for ($lp = 0; $lp < $points; $lp++) {
        /* Normalise first if that's what we are doing */
        if ($normal_type != TRACKFIELD_NORM_NOTHING) {
          $oneset[$yset][$lp] = $oneset[$yset][$lp] - $base_y;
        }
        if ($oneset[$yset][$lp] < $y_min) { $y_min = $oneset[$yset][$lp]; }
        if ($oneset[$yset][$lp] > $y_max) { $y_max = $oneset[$yset][$lp]; }
      }
      /* And now for X */
      if ($oneset[$xset][$points - 1] > $x_max) {
        $x_max = $oneset[$xset][$points - 1];
      }
    }
    $bounds['x_min'] = $x_min;
    $bounds['x_max'] = $x_max;
    $bounds['y_min'] = $y_min;
    $bounds['y_max'] = $y_max;
    /*
     * If we are plotting two or more different sets of data
     * normalise the second set on to fit in the bounds of the first.
     */
    /* TODO: More thought into this */
    if (FALSE && /*$normal_type != TRACKFIELD_NORM_NOTHING && */ count($graphargs['items']) > 1) {
      $base_range = $bounds['y_max'] - $bounds['y_min'];
      $base_min = $bounds['y_min'];
      for ($lp = 1; $lp < count($graphargs['items']); $lp++) {
        $yset = $graphargs['items'][$lp]['yset'];
        /* TODO: figure out when is best to overwrite title */        
//        $dataset[$lp]['title'] = $yset;
        $set_min = min($dataset[$lp][$yset]);
        $set_max = max($dataset[$lp][$yset]);
        $set_range = $set_max - $set_min;
        $set_scale = $set_range / $base_range;
//        print "$yset - $set_min, $set_max, $set_range, $set_scale";
        for ($lp2 = 0; $lp2 < count($dataset[$lp][$yset]); $lp2++) {
          $dataset[$lp][$yset][$lp2] -= $set_min;
          $dataset[$lp][$yset][$lp2] /= $set_scale;
          $dataset[$lp][$yset][$lp2] += $base_min;
        }
      }
    }
  } else {
   /* Just Get bounds for x/y set if given */
    if (isset($xset)) {
      $bounds['x_min'] = min($dataset[0][$xset]);
      $bounds['x_max'] = max($dataset[0][$xset]);
    }
    if (isset($yset)) {
      $bounds['y_min'] = min($dataset[0][$yset]);
      $bounds['y_max'] = max($dataset[0][$yset]);
    }
  }
  return array_merge(array('dataset' => $dataset), $bounds);
}

/*
 * Function to return cache file name/path for give args.
 */

function _trackfield_graph_cache_file($graphargs) {
  $formatter = $graphargs['formatter'];
  $normal_type = $graphargs['normal_type'];
  
  $itemlist = array();
  foreach ($graphargs['items'] as $item) {
    $itemlist[] = $item['nid'] . ',' .
                  $item['vid'] . ',' .
                  $item['field'] . ',' .
                  $item['delta'] . ',' .
                  $item['xset'] . ',' .
                  $item['yset'];
  }
  
  $cache_dir = file_directory_path() . '/' . TRACKFIELD_GRAPH_CACHE_PATH;
  /* Make sure the directory exists - file_create_path will fail if not */
  if (!is_dir($cache_dir)) { mkdir($cache_dir); }
  
  /* MD5 the args because otherwise the name might get too long */
  $cache_file = $cache_dir . '/graph_' .
    md5(join(';', $itemlist)) . '_' . $formatter . '_' . $normal_type . '.png';

  return file_create_path($cache_file);
}

/*
 * This is the main graphing function.
 */

function _trackfield_graph($graphargs) {
  /* Load config */
  $trackfield_format_sizes = variable_get('trackfield_graph_format_sizes', array());
  $trackfield_format_cols = variable_get('trackfield_graph_format_cols', array());
  $formatter = $graphargs['formatter'];
  $config = $trackfield_format_sizes[$formatter];

  /*
   * We want to regen if this file changes due to rules...
   * Or if the formatter config changed
   */
  $modified = max(filemtime(__FILE__), $config['timestamp']);
  $expires_offset = 60*60*24*30; /* HTTP cache expires in one month */

  foreach ($graphargs['items'] as $item) {
    /* Find when these nodes were last changed for caching */
    $sql = "select timestamp from {node_revisions} where nid = %d and vid = %d";
    $result = db_query($sql, $item['nid'], $item['vid']);
    $row = db_fetch_object($result);
    if ($row->timestamp > $modified) {
      $modified = $row->timestamp;
    }
  }

  /* TODO: If $modified is not set then something is wrong! */
  
  /* Return 'Not Modified' if we can */
  if (isset($_SERVER['HTTP_IF_MODIFIED_SINCE'])) {
    /* Remove information after the semicolon and form a timestamp */
    $request_modified = explode(';', $_SERVER['HTTP_IF_MODIFIED_SINCE']);
    $request_modified = strtotime($request_modified[0]);
    
    if (isset($modified) && $modified <= $request_modified) {
      /*
       * If we don't put cache-control and such on this then 304
       * is only sent once and the browser cache cleared, causing
       * the next load to return the whole image.
       */
      header('Content-Type: image/png');
      header('Last-Modified: ' . gmdate('D, d M Y H:i:s', $modified) . ' GMT');
      header('Expires: '. gmdate('D, d M Y H:i:s', time() + $expires_offset) .' GMT');
      header('Cache-Control: public, must-revalidate, max-age=0');
      header('HTTP/1.1 304 Not Modified');
      exit();
    } 
  }
  
  /*
   * We're going to have to return an image now.
   * Tell Apache not to compress it again (if we're using Apache).
   */

  if (function_exists('apache_setenv')) {
    apache_setenv('no-gzip', '1');
  }

  /* Second best - see if we have this in our cache. */
  $cache_file = _trackfield_graph_cache_file($graphargs);
  if (!file_exists($cache_file) ||
      filemtime($cache_file) < $modified) {
    /*
     * Most likely we have to generate now.
     * All is not lost, maybe there are spurious node/fields in the
     * arg list, so regenerate filename after getting the data
     * which checks for thost problems and removes node/fields from
     * the list if they don't exist.
     */

    $graphdata = _trackfield_graph_get_graphdata($graphargs);
    $cache_file = _trackfield_graph_cache_file($graphargs);
  }

  if (!file_exists($cache_file) ||
      filemtime($cache_file) < $modified) {
    /*
     * That's it - generate the graph.
     */
    
    $dataset = $graphdata['dataset'];
    /* TODO: support multiple y data */
    $x_min = $graphdata['x_min'];
    $x_max = $graphdata['x_max'];
    $y_min = $graphdata['y_min'];
    $y_max = $graphdata['y_max'];
    
    /* If distance is > 10000m divide by 1000 and plot as Km */
    /* TODO: Don't assume X is distance */
    /* TODO: Support that anitquated miles/feet system */
    if ($x_max >= TRACKFIELD_METRIC_M_MAX) {
      $x_max /= 1000;
      $dx = 1000;
      $ux = 'Km';
    } else {
      $dx = 1;
      $ux = 'm';
    }
    
    /* Do the graphing */
    
    include_once('./'. drupal_get_path('module', 'trackfield') .'/jpgraph/src/jpgraph.php');
    include_once('./'. drupal_get_path('module', 'trackfield') .'/jpgraph/src/jpgraph_line.php');

    /* Are we building big and resampling? */

    if ($config['resample']) {
      $pixel_multiplier = $config['resample'];
    } else {
      $pixel_multiplier = 1;
    }

    /* Create the graph. */
    $graph = new Graph($config['width'] * $pixel_multiplier,
                       $config['height'] * $pixel_multiplier,
                       'auto');
    $graph->img->SetImgFormat('png');
    /*
     * AntiAliasing doesn't seem to look so nice
     * $graph->img->SetAntiAliasing();
     */
    $graph->SetMarginColor($trackfield_format_cols[t('background')]['col'][0]);
    $graph->SetColor($trackfield_format_cols[t('background')]['col'][0]);
    /* Round the scale to nearest 10 */
    $y_min = floor($y_min / 10) * 10;
    $y_max = ceil($y_max / 10) * 10;
    $graph->SetScale('linlin', $y_min, $y_max, $x_min, $x_max);

    if ($config['legend'] &&
      count($dataset) > 1) {
      $graph->legend->Pos(0.01,0.01, 'left', 'top');
      $graph->legend->SetFont($config['axis_font'], FS_NORMAL, $config['axis_font_size'] * $pixel_multiplier);
    } else {
      $graph->legend->Hide();
    }
    // Put Y Axis on the right if these are hills
    if ($graphargs['normal_type'] == TRACKFIELD_NORM_AS_CLIMB) {
      $graph->yaxis->SetPos('max');
      $graph->yaxis->SetTickSide(SIDE_LEFT); 
      $graph->yaxis->SetTitleSide(SIDE_RIGHT); 
      $graph->yaxis->SetLabelSide(SIDE_RIGHT);
      $graph->SetMargin($config['margins'][1] * $pixel_multiplier,
                        $config['margins'][0] * $pixel_multiplier,
                        $config['margins'][2] * $pixel_multiplier,
                        $config['margins'][3] * $pixel_multiplier);

      $graph->yaxis->SetTitleMargin(($config['margins'][0] - ($config['axis_font_size'] * 2)) * $pixel_multiplier);
    } else {
      $graph->yaxis->SetPos('min');
      $graph->SetMargin($config['margins'][0] * $pixel_multiplier,
                        $config['margins'][1] * $pixel_multiplier,
                        $config['margins'][2] * $pixel_multiplier,
                        $config['margins'][3] * $pixel_multiplier);
      $graph->yaxis->SetTitleMargin(($config['margins'][0] - $config['axis_font_size'] - 2) * $pixel_multiplier);
    }
    $graph->xaxis->SetPos('min');
    $graph->xgrid->Show();
    $graph->ygrid->Show();
    $graph->xaxis->HideTicks(true,true); 
    $graph->yaxis->HideTicks(true,true); 
    $graph->xaxis->SetColor($trackfield_format_cols[t('axis')]['col'][0], $trackfield_format_cols[t('axis')]['col'][0]);
    $graph->yaxis->SetColor($trackfield_format_cols[t('axis')]['col'][0], $trackfield_format_cols[t('axis')]['col'][0]);
    // $graph->title->Set('Hill Comparison');
    // $graph->title->Pos(0, 0, 'left', 'top');
    if ($config['axis']) {
      /* For fixed fonts we have to fix the sizing */
      if ($config['axis_font'] == FF_FONT0) {
        $xfont_pixels = 4;
        $yfont_pixels = 7;
      } else if ($config['axis_font'] == FF_FONT1) {
        $xfont_pixels = 6;
        $yfont_pixels = 9;
      } else if ($config['axis_font'] == FF_FONT2) {
        $xfont_pixels = 7;
        $yfont_pixels = 11;
      } else {
        /* Otherwise just take the configured size */
        $xfont_pixels = $config['axis_font_size'];
        $yfont_pixels = $config['axis_font_size'] * 1.1; /* For line spacing */
      }
      
      $graph->xaxis->title->SetFont($config['axis_font'], FS_NORMAL, $config['axis_font_size'] * $pixel_multiplier);
      $graph->xaxis->title->SetMargin(0);
      $graph->xaxis->title->Set("Distance ($ux)");
      $graph->xaxis->SetTextLabelInterval(1);

      /* Work out how many tick labels we can have */
      $no_label_chars = ceil(log($x_max + 1, 10));
      $no_tick_labels = floor(($config['width'] - $config['margins'][0] - $config['margins'][1]) /
                              ($xfont_pixels * $no_label_chars));
      $ticks = ($x_max - $x_min) / $no_tick_labels;
      /* Then round to nearest 10 */
      $ticks = floor($ticks / 10 + 1) * 10;
      $graph->xaxis->scale->ticks->Set($ticks, $ticks / 2);

      $graph->xaxis->SetFont($config['axis_font'], FS_NORMAL, $config['axis_font_size'] * $pixel_multiplier);
      $graph->xaxis->SetLabelMargin(5 * $pixel_multiplier);

      $graph->yaxis->title->SetFont($config['axis_font'], FS_NORMAL, $config['axis_font_size'] * $pixel_multiplier);
      $graph->yaxis->title->Set('Vertical (m)');
      $graph->yaxis->SetTextLabelInterval(1);
      
      /* Work out how many tick labels we can have */
      $no_tick_labels = floor(($config['height'] - $config['margins'][2] - $config['margins'][3]) / $yfont_pixels);
      $ticks = ($y_max - $y_min) / $no_tick_labels;
      /* Then round to nearest 10 */
      $ticks = floor($ticks / 10 + 1) * 10;
      $graph->yaxis->scale->ticks->Set($ticks, $ticks / 2);

      $graph->yaxis->SetFont($config['axis_font'], FS_NORMAL, $config['axis_font_size'] * $pixel_multiplier);
      $graph->yaxis->SetLabelMargin(5 * $pixel_multiplier);

    } else {
      $graph->yaxis->SetPos(-999999);
      $graph->xaxis->HideLabels();
      $graph->yaxis->HideLabels();
    }
    
    $colours = array('blue', 'cyan', 'gold', 'cornflowerblue', 'firebrick1', 'chocolate1');
    foreach ($dataset as $setno => $oneset) {
      $xset = $graphargs['items'][$setno]['xset'];
      $yset = $graphargs['items'][$setno]['yset'];

      /* Adjust distance range if necessary */
      if ($dx != 1) {
        $xdata = &$oneset[$xset];
        for ($lp = 0; $lp < count($xdata); $lp++) {
          $xdata[$lp] /= $dx;
        }
      }

      /* Create the linear plot */
      $plot = new LinePlot($oneset[$yset], $oneset[$xset]);
      
      /* Defined colour or ours? */
      if ($trackfield_format_cols[$yset]) {
        $set_cols = &$trackfield_format_cols[$yset]['col'];
        $set_fill_cols = &$trackfield_format_cols[$yset]['fill_col'];

        $plot->SetColor($set_cols[0]);
        $our_col = array_shift($set_cols);
        array_push($set_cols, $our_col);
        if ($setno < variable_get('trackfield_graph_multiple_fill_threshold', 0) &&
            count($set_fill_cols) &&
            !empty($set_fill_cols[0])) {
          $plot->SetFillColor($set_fill_cols[0]);
          $our_col = array_shift($set_fill_cols);
          array_push($set_fill_cols, $our_col);
        }
      } else {
        $our_col = array_shift($colours);
        array_push($colours, $our_col);
        $plot->SetColor($our_col);
      }
      $plot->SetWeight($config['weight'] * $pixel_multiplier);
      $plot->SetLegend($oneset['title']);

      /* Add the plot to the graph */
      $graph->Add($plot);
    }
    
    /*
     * There seems to be problem with this:
     *   $graph->img->Stream($cache_file);
     *   $graph->Stroke($cache_file);
     */
    $img =  $graph->Stroke(_IMG_HANDLER);
    // Resample if necessary
    if ($pixel_multiplier != 1) {
      $tmp_img = imagecreatetruecolor(
        $config['width'],
        $config['height']);
        imagecopyresampled($tmp_img, $img, 0, 0, 0, 0,
          $config['width'],
          $config['height'],
          $config['width'] * $pixel_multiplier,
          $config['height'] * $pixel_multiplier);
        imagepng($tmp_img, $cache_file, 9);
    } else {
     imagepng($img, $cache_file, 9);
   }
  }

  $headers = array(
    'Content-Type: image/png',
    'Content-Length: ' . filesize($cache_file),
    'Last-Modified: ' . gmdate('D, d M Y H:i:s', $modified) . ' GMT',
    'Expires: '. gmdate('D, d M Y H:i:s', time() + $expires_offset) .' GMT',
    'Cache-Control: public, must-revalidate, max-age=0');
  file_transfer($cache_file, $headers);
}

?>
