<div id="merce-landing-page">
	<div id="front-wrapper-top">
	  <div id="front-title"></div>
	  <div id="front-portrait"></div>
	  <div id="front-bio">
	    <?php print $bio; ?>
	  </div>
	  <div id="front-fjm-menu">
	    <?php print $menu; ?>
	  </div>
	  <div id="front-carousel">
      <?php print $carousel; ?>
	  </div>
	</div>
	<div id="front-wrapper-bottom">
	  <div id="front-browse">
	    <h2 class="front-title"><?php print t('Browse'); ?></h2>
	    <div id="front-browse-wrapper">
	      <?php print $browse; ?>
	    </div>
	  </div>
          <?php if ($search_form != ''): ?>
	  <div id="front-search-form">
            <h2 class="front-title"><?php print t('Search'); ?></h2>
	    <div id="front-search-wrapper">
	      <?php print $search_form; ?>
	    </div>
	  </div>
	<?php endif; ?>
	</div>
</div>
