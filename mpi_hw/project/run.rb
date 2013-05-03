#!/usr/bin/env ruby
# Ruby script to run reorthogonalization.
# Spares the user from giving the number of vectors 
# and the length of vectors as inputs.
# Usage:
#   1. Change the # of cores in this file.
#   2. Run the script by 
#       run.rb [basis vector 1], [basis vector 2], ... [basis vector k], [new vector]
#      e.g.
#       run.rb 2.0 3.0 -1.0, -1.0 1.0 1.0, 3 4 5
#       will start with basis {(2,3,-1), (-1,1,1)}
#       and orthogonalize the last vector (3,4,5)
#       so that the new basis {(2,3,-1), (-1,1,1), q}
#       is an orthogonal basis.

cores = 8

argv = ARGV * ' '
vectors = argv.split(',')
num_vecs = vectors.length
# The dimension of the basis
basis_dim = num_vecs - 1
# Length of each vector
vector_len = vectors.first.split(' ').length

# check if we can run reorthogonalization
puts "\# of vectors: #{num_vecs}"
puts "Length of each vector #{vector_len}"
if (basis_dim + 1> vector_len)
  puts 'We have dim(basis) + 1 > len(vector).'
  abort "The new vector can't be reorthogonalized."
end

command = "mpirun -np #{cores} p_reorthogonalize "
command << basis_dim.to_s + ' '
command << vector_len.to_s + ' '
command << vectors * ' '
puts "Inputs are ready. Replacing the process with system call:"
puts command
system(command)
