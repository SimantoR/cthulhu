
var loadaakara = (() => {
  var _scriptDir = typeof document !== 'undefined' && document.currentScript ? document.currentScript.src : undefined;
  
  return (
function(loadaakara) {
  loadaakara = loadaakara || {};



  return loadaakara.ready
}
);
})();
if (typeof exports === 'object' && typeof module === 'object')
  module.exports = loadaakara;
else if (typeof define === 'function' && define['amd'])
  define([], function() { return loadaakara; });
else if (typeof exports === 'object')
  exports["loadaakara"] = loadaakara;