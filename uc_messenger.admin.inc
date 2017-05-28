<?php

/**
 * @file
 * Formulaire de reglage administration du module.
 */

/*
VARIABLES MEMORISEE:

uc_messenger_codespostaux					liste des codes postaux autorises, sepeares par des virgules
uc_messenger_taux							taux de reduction/augmentation applique a la commande suivant ce mode de livraison
uc_messenger_tempsPreparation				delai minimum entre la commande et la livraison
uc_messenger_creneau_X_activation			ou X est le numero de creneau
uc_messenger_creneau_X_debut_heure			heure de debut de creneau
uc_messenger_creneau_X_debut_mn			minutes de debut de creneau
uc_messenger_creneau_X_fin_heure			heure de fin de creneau
uc_messenger_creneau_X_fin_mn				minutes de fin de creneau
uc_messenger_creneau_X_orderMin			montant de commande minimum
uc_messenger_creneau_X_jours				tableau des jours de la semaine (numero de jour) ouverts pour ce creneau
 */
 

define('NBRE_CRENEAU_HORAIRES', 2);

function uc_messenger_settings_form($form, &$form_state) {
	
	$form['#validate'][]	= 'uc_messenger_settings_form_validate';
	$form['#submit'][]		= 'uc_messenger_settings_form_submit';
	


//!==========================================================================================
//! 						CODE POSTAUX ACCEPTES
//==========================================================================================

	$form['uc_messenger_codespostaux'] = array(
		'#type' 	=> 'textfield',
		'#required'	=> TRUE,
		'#title' 	=> t('Code postaux acceptés pour la livraison'),
		'#description' => 'Liste des codes postaux acceptés, séparés par des virgules',
		'#default_value' => variable_get('uc_messenger_codespostaux'),	
	);

//!==========================================================================================
//! 						TAUX APPLIQUE
//==========================================================================================

	$form['uc_messenger_taux'] = array(	  
		'#type' => 'select',
		'#required' => TRUE,    
		'#title' => t('Taux appliqué'),
		'#options' => array(
			-20 => -20,		
			-15 => -15,		
			-10 => -10,		
			-5 => -5,		
			0 => 0,
			5 => 5,
			10 => 10,
			15 => 15,
			20 => 20,                 
		),       
		'#default_value' => variable_get('uc_messenger_taux', 10),	
		'#description' => t('Ce taux, sera appliqué au Total HT + TVA    (ATTENTION " - " EST UNE RÉDUCTION, sinon il\'a augmentation ! )'),     
		'#field_suffix' => ' %', 
	);

//!==========================================================================================
//! 						TEMPS DE PREPARATION MINIMUM
//==========================================================================================	
	$form['uc_messenger_tempsPreparation'] = array(	  
		'#type' => 'select',
		'#required' => TRUE,    
		'#title' => t('Délai'),
		'#options' => array(
			0  => 0,		
			10 => 10,		
			15 => 15,		
			20 => 20,		
			25 => 25,
			30 => 30,
			35 => 35,
			40 => 40,
			45 => 45,                 
		),       
		'#default_value' => variable_get('uc_messenger_tempsPreparation', 20),	
		'#description' => t('Temps de préparation avant la livraison chez le client'),     
		'#field_suffix' => ' mn', 
	);

//!==========================================================================================
//! 						CRENEAUX HORAIRES ET MONTANTS DE COMMANDE MINIMUM
//==========================================================================================	

	for($i = 1; $i <= NBRE_CRENEAU_HORAIRES; $i++){		
			_uc_messenger_settings_creneauhoraire($form, $i);
			}

	$form['uc_messenger_submit'] = array(
		'#type' 			=> 'submit',
		'#value'			=> t('Enregistrer')
	);
	return $form;
}

//!==========================================================================================
//! 						_VALIDATE
//==========================================================================================

function uc_messenger_settings_form_validate($form, &$form_state) {
	if( _uc_messenger_codespostaux( $form_state['values']['uc_messenger_codespostaux']) == FALSE ){
		form_set_error( 'uc_messenger_codespostaux' , t('Vous devez saisir un ou des codes postaux valides séparés par des virgules.') );
	}
}

//!==========================================================================================
//! 						_SUBMIT
//==========================================================================================

function uc_messenger_settings_form_submit($form, &$form_state) {

	$lesCodesPostaux = _uc_messenger_codespostaux( $form['uc_messenger_codespostaux']['#value']) ;
	variable_set( 'uc_messenger_codespostaux'		, $lesCodesPostaux);
	variable_set( 'uc_messenger_taux'				, $form['uc_messenger_taux']['#value']);
	variable_set( 'uc_messenger_tempsPreparation'	, $form['uc_messenger_tempsPreparation']['#value']);

	for($i = 1; $i <= NBRE_CRENEAU_HORAIRES; $i++) {
		variable_set( 'uc_messenger_creneau_' . $i . '_activation', 	$form['uc_messenger_creneau']['activation-' . $i ]['#value']);	
		if(  $form['uc_messenger_creneau']['activation-' . $i]['#value']  ==  1 ) {
			variable_set( 'uc_messenger_creneau_' . $i . '_jours'			, $form['uc_messenger_creneau']['data-' . $i ]['jours'  			. $i ]['#value']);
			variable_set( 'uc_messenger_creneau_' . $i . '_debut_heure'	, $form['uc_messenger_creneau']['data-' . $i ]['debutHeure'  		. $i ]['#value']);
			variable_set( 'uc_messenger_creneau_' . $i . '_debut_mn'		, $form['uc_messenger_creneau']['data-' . $i ]['debutMinute' 		. $i ]['#value']);
			variable_set( 'uc_messenger_creneau_' . $i . '_fin_heure'		, $form['uc_messenger_creneau']['data-' . $i ]['finHeure'    		. $i ]['#value']);
			variable_set( 'uc_messenger_creneau_' . $i . '_fin_mn'			, $form['uc_messenger_creneau']['data-' . $i ]['finMinute'   		. $i ]['#value']);
			variable_set( 'uc_messenger_creneau_' . $i . '_orderMin'		, $form['uc_messenger_creneau']['data-' . $i ]['commandeMinimum'	. $i ]['#value']);	
		}
	}

}

//!==========================================================================================
//! 						DIVERS
//==========================================================================================

//
//	verifie les valeurs saisies, renvoie la chaine nettoyee si au moins une valeur correcte
//
function _uc_messenger_codespostaux( $string ){
	$codepostauxArray = explode( "," , $string , 50 );
	if( array_count_values($codepostauxArray) > 1 ){
		$retour = array();
		foreach( $codepostauxArray as $cle => $valeur){
			$chaine =  trim($codepostauxArray[$cle]);
			if( is_numeric($chaine) ) {
				$retour[] = $chaine;
			}
		}
		if( array_count_values( $retour ) >= 1 ){
			return implode( "," , $retour );					
		}
	}else{
		return FALSE;
	}
}

//
//! Form Creneau horaire
//
function _uc_messenger_settings_creneauhoraire( &$form, $NUMcreneauHoraire ) {

	$jours = array(
		1 => t("lundi"),
		2 => t("mardi"),
		3 => t("mercredi"),
		4 => t("jeudi"),
		5 => t("vendredi"),
		6 => t("samedi"),
		7 => t("dimanche")
	); 
	
	variable_get('uc_messenger_creneau_' . $NUMcreneauHoraire . '_activation') == 1 ? $checked = 1 : $checked = 0;

	$form['uc_messenger_creneau']['activation-' . $NUMcreneauHoraire] = array(	  
		'#type' 		=> 'checkbox',
		'#title' 		=> t('Utiliser le créneau horaire') . ' ' . $NUMcreneauHoraire,
 		'#default_value'=> $checked
	);
				
	$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire] = array(	  
		'#type' 		=> 'fieldset',
		'#required' 	=> TRUE,    
		'#title' 		=> t('Paramètres du montant de commande minimale numéro') . ' ' . $NUMcreneauHoraire,
		'#collapsible' 	=> FALSE,
		'#states' => array(
			'visible' => array(
			':input[name="activation-' . $NUMcreneauHoraire. '"]' => array('checked' => TRUE),
			),			
		),		
	);

			$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['jours' . $NUMcreneauHoraire] = array(	  
				'#type' 		=> 'checkboxes',
				'#title' 		=> t('Jours'),
				'#options' 		=> $jours,
				'#default_value' => variable_get( 'uc_messenger_creneau_' . $NUMcreneauHoraire . '_jours'),
				'#prefix' 		=> '<div class="date-livraison-creneau-jours">',
				'#suffix' 		=> '</div>',
			);							

			$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['debutLigneHoraire'] = array(	  
				'#markup' 		=> '<div class="date-livraison-creneau creneau-' . $NUMcreneauHoraire . ' activation">' . "\n", 
			);

			$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['debutHeure' . $NUMcreneauHoraire] = array(	  
				'#type' 		=> 'select',
				'#title' 		=> t('Début'),
				'#options' 		=> array(), 
				'#default_value'=> variable_get( 'uc_messenger_creneau_' . $NUMcreneauHoraire . '_debut_heure', 6),		
				'#field_suffix' => ' h',
				'#prefix' 		=> '<div class="date-livraison-creneau creneau-' . $NUMcreneauHoraire . ' debut">' . "\n", 
			);
			for($i = 0; $i <= 23; $i++ ){ $form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['debutHeure' . $NUMcreneauHoraire]['#options'][$i] = $i; }
			
			$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['debutMinute' . $NUMcreneauHoraire] = array(	  
				'#type' 		=> 'select',
				'#options' 		=> array(), 
				'#default_value'=> variable_get( 'uc_messenger_creneau_' . $NUMcreneauHoraire . '_debut_mn', 0),				
				'#field_suffix' => ' mn', 
				'#suffix' 		=> '</div>' . "\n", 				
			);
			for($i = 0; $i <= 59; $i++ ){ $form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['debutMinute' . $NUMcreneauHoraire]['#options'][$i] = $i; }


			$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['finHeure' . $NUMcreneauHoraire] = array(	  
				'#type' 		=> 'select',
				'#title' 		=> t('Fin'),
				'#options' 		=> array(), 
				'#default_value'=> variable_get( 'uc_messenger_creneau_' . $NUMcreneauHoraire . '_fin_heure', 15),				
				'#field_suffix' => ' h',
				'#prefix' 		=> '<div class="date-livraison-creneau creneau-' . $NUMcreneauHoraire . ' fin">' . "\n", 
			);
			for($i = 0; $i <= 23; $i++ ){ $form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['finHeure' . $NUMcreneauHoraire]['#options'][$i] = $i; }
			
			
			$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['finMinute' . $NUMcreneauHoraire] = array(	  
				'#type' 		=> 'select',				
				'#options' 		=> array(),
				'#default_value'=> variable_get( 'uc_messenger_creneau_' . $NUMcreneauHoraire . '_fin_mn', 59),				 
				'#field_suffix' => ' mn', 
				'#suffix' 		=> '</div>' . "\n", 				
			);
			for($i = 0; $i <= 59; $i++ ){ $form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['finMinute' . $NUMcreneauHoraire]['#options'][$i] = $i; }


			$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['finLigneHoraire'] = array(	  
				'#markup' 		=> '</div>' . "\n", 
			);

			$form['uc_messenger_creneau']['data-' . $NUMcreneauHoraire]['commandeMinimum' . $NUMcreneauHoraire] = array(	  
				'#type' 		=> 'textfield',
				'#title'		=> t('Minimum de commande'),
				'#description'	=> t('Le séparateur décimal est le point.'),				
				'#field_suffix' => variable_get('uc_currency_sign', '€'), 
				'#default_value'=> variable_get( 'uc_messenger_creneau_' . $NUMcreneauHoraire . '_orderMin', 15 ),
				'#size' 		=> 5, 
				'#maxlength' 	=> 5,
				'#prefix' 		=> '<div class="date-livraison-creneau creneau-' . $NUMcreneauHoraire . ' montant">' . "\n", 				
				'#suffix' 		=> '</div>' . "\n",				
			);
			return ;
}