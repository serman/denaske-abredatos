<?php

dpm($node,'node');

?>

<div id="node-<?php print $node->nid; ?>" class="node <?php print $node_classes; ?>">
  <div class="inner">
    <?php print $picture ?>

    <?php if ($submitted && FALSE): ?>
    <div class="meta">
      <span class="submitted"><?php print $submitted ?></span>
    </div>
    <?php endif; ?>

<?php if(!$teaser){ ?>

    <?php if ($node_top && !$teaser): ?>
    <div id="node-top" class="node-top row nested">
      <div id="node-top-inner" class="node-top-inner inner">
        <?php print $node_top; ?>
      </div><!-- /node-top-inner -->
    </div><!-- /node-top -->
    <?php endif; ?>
    
        <?php if ($links): ?>
    <div class="links">
      <?php print $links; ?>
    </div>
    <?php endif; ?>
  </div><!-- /inner -->    
    
  <div class="content clearfix">
      
      <?php
      print $node->content['mapas']['#value'];
      ?>
      
      <div id="fotos">
      
      <?php print views_embed_view('espacios_fotos_notas', 'default', $node->nid); ?>

    </div>
      
    <div id="information">
      <h2>Información</h2>      
      
      </div>
      
      <div id="notas">
      <?php
      
      //1 Denuncia
      //2 Publica
      //3 Privada
      
     // print $node->content['notas_espacio']['#value'];       
            
      
      dpm($args);
      
        $tabs['publicos'] = array(
          'title' => 'Publicos',
          'type' => 'view',
          'vid' => 'notas_reference_espacio',
          'display' => 'block_1',
          'args' => $node->nid,
        );
      
        if($user->uid){ //TODO: Cambiar a current user
          
        $tabs['privado'] = array(
          'title' => 'Privado',
          'type' => 'view',
          'vid' => 'notas_reference_espacio',
          'display' => 'block_3',
          'args' => array($node->nid,3),
        );
          
          
        }    
        
        $tabs['denuncia'] = array(
          'title' => 'Denuncia',
          'type' => 'view',
          'vid' => 'notas_reference_espacio',
          'display' => 'block_2',
          'args' => array($node->nid,2),
        );
        
      
      
        $quicktabs['qtid'] = 'C' . $node->nid;
        $quicktabs['tabs'] = $tabs;
        $quicktabs['style'] = 'Zen';
        $quicktabs['ajax'] = TRUE;
        print theme('quicktabs', $quicktabs);
      
      
      
      ?>
      </div>      
    </div>

    
<? }else{//teaser ?>

<div id="map-teaser"></div>

    <?php if ($page == 0): ?>
    <h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
    <?php endif; ?>







<? } ?>


    <?php if ($terms): ?>
    <div class="terms">
      <?php print $terms; ?>
    </div>
    <?php endif;?>



  <?php if ($node_bottom && !$teaser): ?>
  <div id="node-bottom" class="node-bottom row nested">
    <div id="node-bottom-inner" class="node-bottom-inner inner">
      <?php print $node_bottom; ?>
    </div><!-- /node-bottom-inner -->
  </div><!-- /node-bottom -->
  <?php endif; ?>
</div><!-- /node-<?php print $node->nid; ?> -->
