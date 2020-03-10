#~ Hello World in Ruby
#~ Printing is done by R, not Ruby
require 'ffi'

module R
  extend FFI::Library
  libdir = `Rscript -e \'cat(find.package("RInside"))\'`
  ffi_lib "#{libdir}/lib/libRInside.so"
  attach_function :setupRinC, [ ], :void
  attach_function :evalQuietlyInR, [ :string ], :void
  attach_function :teardownRinC, [ ], :void
end

R.setupRinC()
R.evalQuietlyInR("print('Hello from R!')")
R.teardownRinC()
