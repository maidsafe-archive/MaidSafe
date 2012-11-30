function ParseQueryString(query_string) {
  var nvpair = {};
  var qs = query_string.replace('?', '');
  var pairs = qs.split('&');
  $.each(pairs, function(i, v){
    var pair = v.split('=');
    nvpair[pair[0]] = pair[1];
  });
  return nvpair;
}