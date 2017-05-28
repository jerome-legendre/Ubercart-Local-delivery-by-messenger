
(function($) {
	Drupal.behaviors.hidePanelLivraison = {
		attach: function(context, settings){
			if (     $( '[id^="edit-panes-quotes-quotes-quote-option-livraison-0"]' ).is(':checked')      ){    
				jQuery('#horaireLivraisons-pane').show()
			} else {
				jQuery('#horaireLivraisons-pane').hide()
			}
		}
	}
})(jQuery)

    