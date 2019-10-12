
if ((typeof Haravan) === 'undefined') {
  Haravan = {};
}

Haravan.cultures = [
    {
        code: 'vi-VN',
        thousands: ',',
        decimal: '.',
        numberdecimal: 0,
        money_format: '{{amount}}'
    },
    {
        code: 'en-US',
        thousands: ',',
        decimal: '.',
        numberdecimal: 2,
        money_format: '{{amount}}'
    }
]

Haravan.getCulture = function (code) {
    var culture;
    for (n = 0; n < Haravan.cultures.length; n++) {
        if (Haravan.cultures[n].code == code) {
            culture = Haravan.cultures[n];
            break;
        }
    }
    if (!culture) {
        culture = Haravan.cultures[0];
    }
    return culture;
}

Haravan.format = Haravan.getCulture(Haravan.culture)
Haravan.money_format = "{{amount}}";
/* 

Events (override!)

Example override:
  ... add to your theme.liquid's script tag....

  Haravan.onItemAdded = function(line_item) {
    $('message').update('Added '+line_item.title + '...');
  }
  
*/

Haravan.onError = function(XMLHttpRequest, textStatus) {
  // Haravan returns a description of the error in XMLHttpRequest.responseText.
  // It is JSON.
  // Example: {"description":"The product 'Amelia - Small' is already sold out.","status":500,"message":"Cart Error"}
  var data = eval('(' + XMLHttpRequest.responseText + ')');
  if (!!data.message) {
    alert(data.message + '(' + data.status  + '): ' + data.description);
  } else {
    alert('Error : ' + Haravan.fullMessagesFromErrors(data).join('; ') + '.');
  }
};

Haravan.fullMessagesFromErrors = function(errors) {
  var fullMessages = [];
  jQuery.each(errors, function(attribute, messages) {
    jQuery.each(messages, function(index, message) {
      fullMessages.push(attribute + ' ' + message);
    });
  });
  return fullMessages
}

Haravan.onCartUpdate = function(cart) {
  alert('There are now ' + cart.item_count + ' items in the cart.');
};  

Haravan.onCartShippingRatesUpdate = function(rates, shipping_address) {
  var readable_address = '';
  if (shipping_address.zip) readable_address += shipping_address.zip + ', ';
  if (shipping_address.province) readable_address += shipping_address.province + ', ';
  readable_address += shipping_address.country
  alert('There are ' + rates.length + ' shipping rates available for ' + readable_address +', starting at '+ Haravan.formatMoney(rates[0].price) +'.');
};  

Haravan.onItemAdded = function(line_item) {
  alert(line_item.title + ' was added to your shopping cart.');
};

Haravan.onProduct = function(product) {
  alert('Received everything we ever wanted to know about ' + product.title);
};


Haravan.formatMoney = function (cents, format) {
  cents = cents / 100;
  if (typeof cents == 'string') cents = cents.replace(Haravan.format.thousands, '');
  var value = '';
  var patt = /\{\{\s*(\w+)\s*\}\}/;
  var formatString = (format || this.money_format);

  function addCommas(moneyString) {
      return moneyString.replace(/(\d)(?=(\d\d\d)+(?!\d))/g, '$1' + Haravan.format.thousands);
  }
  switch(formatString.match(patt)[1]) {
  case 'amount':
      value = addCommas(floatToString(cents, Haravan.format.numberdecimal));
    break;
  case 'amount_no_decimals':
    value = addCommas(floatToString(cents, 0));
    break;
  case 'amount_with_comma_separator':
      value = floatToString(cents, Haravan.format.numberdecimal).replace(/\./, ',');
    break;
  case 'amount_no_decimals_with_comma_separator':
    value = addCommas(floatToString(cents, 0)).replace(/\./, ',');
    break;
  }
  return formatString.replace(patt, value);
};

Haravan.resizeImage = function(image, size) {
     try {
        if (size == 'original') {
            return image;
        } else {
            var matches = image.match(/(.*\/[\w\-\_\.]+)\.(\w{2,4})/);
            return matches[1].replace(/http:/g, '').replace(/https:/g, '') + '_' + size + '.' + matches[2];
        }
    } catch (e) {
        return image.replace(/http:/g, '').replace(/https:/g, '');
    }
};

Haravan.addItem = function(variant_id, quantity, callback) {
  var quantity = quantity || 1;
  var params = {
    type: 'POST',
    url: '/cart/add.js',
    data: 'quantity=' + quantity + '&id=' + variant_id,
    dataType: 'json',
    success: function(line_item) { 
      if ((typeof callback) === 'function') {
        callback(line_item);
      }
      else {
        Haravan.onItemAdded(line_item);
      }
    },
    error: function(XMLHttpRequest, textStatus) {
      Haravan.onError(XMLHttpRequest, textStatus);
    }
  };
  jQuery.ajax(params);
};

Haravan.addItemFromForm = function(form_id, callback) {
    var params = {
      type: 'POST',
      url: '/cart/add.js',
      data: jQuery('#' + form_id).serialize(),
      dataType: 'json',
      success: function(line_item) { 
        if ((typeof callback) === 'function') {
          callback(line_item);
        }
        else {
          Haravan.onItemAdded(line_item);
        }
      },
      error: function(XMLHttpRequest, textStatus) {
        Haravan.onError(XMLHttpRequest, textStatus);
      }
    };
    jQuery.ajax(params);
};

Haravan.getCart = function(callback) {
  jQuery.getJSON('/cart.js', function (cart, textStatus) {
    if ((typeof callback) === 'function') {
      callback(cart);
    }
    else {
      Haravan.onCartUpdate(cart);
    }
  });
};

Haravan.getCartShippingRatesForDestination = function(shipping_address, callback) {
  var params = {
    type: 'GET',
    url: '/cart/shipping_rates.json',
    data: Haravan.param({'shipping_address': shipping_address}),
    dataType: 'json',
    success: function(response) { 
      rates = response.shipping_rates
      if ((typeof callback) === 'function') {
        callback(rates, shipping_address);
      }
      else {
        Haravan.onCartShippingRatesUpdate(rates, shipping_address);
      }
    },
    error: function(XMLHttpRequest, textStatus) {
      Haravan.onError(XMLHttpRequest, textStatus);
    }
  }
  jQuery.ajax(params);
};

Haravan.getProduct = function(handle, callback) {
  jQuery.getJSON('/products/' + handle + '.js', function (product, textStatus) {
    if ((typeof callback) === 'function') {
      callback(product);
    }
    else {
      Haravan.onProduct(product);
    }
  });
};

Haravan.changeItem = function(variant_id, quantity, callback) {
  var params = {
    type: 'POST',
    url: '/cart/change.js',
    data:  'quantity='+quantity+'&id='+variant_id,
    dataType: 'json',
    success: function(cart) { 
      if ((typeof callback) === 'function') {
        callback(cart);
      }
      else {
        Haravan.onCartUpdate(cart);
      }
    },
    error: function(XMLHttpRequest, textStatus) {
      Haravan.onError(XMLHttpRequest, textStatus);
    }
  };
  jQuery.ajax(params);
};

Haravan.removeItem = function(variant_id, callback) {
  var params = {
    type: 'POST',
    url: '/cart/change.js',
    data:  'quantity=0&id='+variant_id,
    dataType: 'json',
    success: function(cart) { 
      if ((typeof callback) === 'function') {
        callback(cart);
      }
      else {
        Haravan.onCartUpdate(cart);
      }
    },
    error: function(XMLHttpRequest, textStatus) {
      Haravan.onError(XMLHttpRequest, textStatus);
    }
  };
  jQuery.ajax(params);
};

Haravan.clear = function(callback) {
  var params = {
    type: 'POST',
    url: '/cart/clear.js',
    data:  '',
    dataType: 'json',
    success: function(cart) { 
      if ((typeof callback) === 'function') {
        callback(cart);
      }
      else {
        Haravan.onCartUpdate(cart);
      }
    },
    error: function(XMLHttpRequest, textStatus) {
      Haravan.onError(XMLHttpRequest, textStatus);
    }
  };
  jQuery.ajax(params);
};

Haravan.updateCartFromForm = function(form_id, callback) {
  var params = {
    type: 'POST',
    url: '/cart/update.js',
    data: jQuery('#' + form_id).serialize(),
    dataType: 'json',
    success: function(cart) {
      if ((typeof callback) === 'function') {
        callback(cart);
      }
      else {
        Haravan.onCartUpdate(cart);
      }
    },
    error: function(XMLHttpRequest, textStatus) {
      Haravan.onError(XMLHttpRequest, textStatus);
    }
  };
  jQuery.ajax(params);
};

Haravan.updateCartAttributes = function(attributes, callback) {
  var data = '';
  // If attributes is an array of the form:
  // [ { key: 'my key', value: 'my value' }, ... ]
  if (jQuery.isArray(attributes)) {
    jQuery.each(attributes, function(indexInArray, valueOfElement) {
      var key = attributeToString(valueOfElement.key);
      if (key !== '') {
        data += 'attributes[' + key + ']=' + attributeToString(valueOfElement.value) + '&';
      }
    });
  }
  // If attributes is a hash of the form:
  // { 'my key' : 'my value', ... }
  else if ((typeof attributes === 'object') && attributes !== null) {
    jQuery.each(attributes, function(key, value) {
        data += 'attributes[' + attributeToString(key) + ']=' + attributeToString(value) + '&';
    });
  }
  var params = {
    type: 'POST',
    url: '/cart/update.js',
    data: data,
    dataType: 'json',
    success: function(cart) {
      if ((typeof callback) === 'function') {
        callback(cart);
      }
      else {
        Haravan.onCartUpdate(cart);
      }
    },
    error: function(XMLHttpRequest, textStatus) {
      Haravan.onError(XMLHttpRequest, textStatus);
    }
  };
  jQuery.ajax(params);
};

// ---------------------------------------------------------
// POST to cart/update.js returns the cart in JSON.
// ---------------------------------------------------------
Haravan.updateCartNote = function(note, callback) {
  var params = {
    type: 'POST',
    url: '/cart/update.js',
    data: 'note=' + attributeToString(note),
    dataType: 'json',
    success: function(cart) {
      if ((typeof callback) === 'function') {
        callback(cart);
      }
      else {
        Haravan.onCartUpdate(cart);
      }
    },
    error: function(XMLHttpRequest, textStatus) {
      Haravan.onError(XMLHttpRequest, textStatus);
    }
  };
  jQuery.ajax(params);
};


if (jQuery.fn.jquery >= '1.4') {
  Haravan.param = jQuery.param;
} else {
  Haravan.param = function( a ) {
    var s = [],
      add = function( key, value ) {
        // If value is a function, invoke it and return its value
        value = jQuery.isFunction(value) ? value() : value;
        s[ s.length ] = encodeURIComponent(key) + "=" + encodeURIComponent(value);
      };
  
    // If an array was passed in, assume that it is an array of form elements.
    if ( jQuery.isArray(a) || a.jquery ) {
      // Serialize the form elements
      jQuery.each( a, function() {
        add( this.name, this.value );
      });
    
    } else {
      for ( var prefix in a ) {
        Haravan.buildParams( prefix, a[prefix], add );
      }
    }

    // Return the resulting serialization
    return s.join("&").replace(/%20/g, "+");
  }

  Haravan.buildParams = function( prefix, obj, add ) {
    if ( jQuery.isArray(obj) && obj.length ) {
      // Serialize array item.
      jQuery.each( obj, function( i, v ) {
        if ( rbracket.test( prefix ) ) {
          // Treat each array item as a scalar.
          add( prefix, v );

        } else {
          Haravan.buildParams( prefix + "[" + ( typeof v === "object" || jQuery.isArray(v) ? i : "" ) + "]", v, add );
        }
      });
      
    } else if ( obj != null && typeof obj === "object" ) {
      if ( Haravan.isEmptyObject( obj ) ) {
        add( prefix, "" );

      // Serialize object item.
      } else {
        jQuery.each( obj, function( k, v ) {
          Haravan.buildParams( prefix + "[" + k + "]", v, add );
        });
      }
          
    } else {
      // Serialize scalar item.
      add( prefix, obj );
    }
  }
  
  Haravan.isEmptyObject = function( obj ) {
    for ( var name in obj ) {
      return false;
    }
    return true;
  }
  
  Haravan.ExpressionSpecialChars = [
      { key: '(', val: '%26' },
      { key: ')', val: '%27' },
      { key: '|', val: '%28' },
      { key: '-', val: '%29' },
      { key: '&', val: '%30' }
  ];

  Haravan.encodeExpressionValue = function (val) {
      if((typeof val) !== 'string' || val == null || val == "")
          return val;

      val = val.replace('%', '%25');
      for (n = 0; n < Haravan.ExpressionSpecialChars.length ; n++) {
          var char = Haravan.ExpressionSpecialChars[n];
          val = val.replace(char.key, char.val);
      }

      return val;
  }
}


/* Used by Tools */

function floatToString(numeric, decimals) {
    var amount = numeric.toFixed(decimals).toString();
    amount.replace('.',Haravan.decimal);
    if(amount.match('^[\.' + Haravan.decimal +']\d+')) {return "0"+amount; }
  else { return amount; }
}

/* Used by API */

function attributeToString(attribute) {
  if ((typeof attribute) !== 'string') {
    // Converts to a string.
    attribute += '';
    if (attribute === 'undefined') {
      attribute = '';
    }
  }
  // Removing leading and trailing whitespace.
  return jQuery.trim(attribute);
}
