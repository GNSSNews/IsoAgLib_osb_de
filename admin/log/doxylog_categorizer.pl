#!/usr/bin/perl
#
# Description: categorizes and splits the log generated by doxygen into seperate files
# Created    : 14.01.2008 (changed: 15.01.2008)
# Author     : F. Muecke, OSB AG
#
# Logfile format: 
#   	WARN_FORMAT		= "$file:$line :$text "
#	WARNINGS		= YES
#	WARN_IF_UNDOCUMENTED	= YES
#	WARN_IF_DOC_ERROR	= YES
#                
if( @ARGV == 0 )
{
  $fname = "doxywarnings.txt";
}
else
{
  $fname = @ARGV[0];
}
open(FILE, $fname) or die "Can't find file $FILE: $!\nusage: doxylog_categorizer.pl doxylog-file\n\n";
@raw=<FILE>;
close(FILE);

while ( defined( $line = shift(@raw) ) )
{
  if ( $line =~ m/member/ && $line =~ m/class/ && $line =~ m/found/ )
  {
    $no_class_member .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $no_class_member .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/belongs to/ && $line =~ m/different groups/ )
  {
    $multiple_groups .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $multiple_groups .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }
  
  elsif ( $line =~ m/matching file member/ )
  {
    $no_file_member .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $no_file_member .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/resolve reference/ )
  {
    $unresolved_ref .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $unresolved_ref .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/parameter/ && $line =~ m/not documented/ )
  {
    $param_undoc .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $param_undoc .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/documented function/ && $line =~ m/not declared or defined/ )
  {
    $func_nodef .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $func_nodef .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/argument/ && $line =~ m/of command \@param is not found in the argument list/ )
  {
    $param_not_found .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $param_not_found .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/comment block/ )
  {
    $comment_block .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $comment_block .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/parsing initializer list/ )
  {
    $parsing_initializer .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $parsing_initializer .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/include file/ && $line =~ m/not found/ )
  {
    $include_not_found .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $include_not_found .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/parse error/ )
  {
    $parse_error .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $parse_error .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/\\file statement/ )
  {
    $file_tag_wrong .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|Error |<unknown)/ )
      {
        $file_tag_wrong .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }

  elsif ( $line =~ m/^Error opening map file/ )
  {
    $error_map_file .= $line;

    while( defined( $line = shift( @raw ) ) )
    {
      if ( $line !~ m/^(\/|<unknown)/ )
      {
        $error_map_file .= $line;
      }
      else
      {
        unshift( @raw, $line );
	last;
      }
    }
  }
  else
  {
    $misc .= $line;
  }
}

#
# write output
#
open(FILE, ">$fname".".multiple_groups.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $multiple_groups);
close(FILE);

open(FILE, ">$fname".".misc.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $misc);
close(FILE);

open(FILE, ">$fname".".no_class_member.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $no_class_member);
close(FILE);

open(FILE, ">$fname".".no_file_member.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $no_file_member);
close(FILE);

open(FILE, ">$fname".".unresolved_ref.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $unresolved_ref);
close(FILE);

open(FILE, ">$fname".".param_undoc.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $param_undoc);
close(FILE);

open(FILE, ">$fname".".func_nodef.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $func_nodef);
close(FILE);

open(FILE, ">$fname".".param_not_found.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $param_not_found);
close(FILE);

open(FILE, ">$fname".".comment_block.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $comment_block);
close(FILE);

open(FILE, ">$fname".".parsing_initializer.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $parsing_initializer);
close(FILE);

open(FILE, ">$fname".".include_not_found.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $include_not_found);
close(FILE);

open(FILE, ">$fname".".parse_error.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $parse_error);
close(FILE);

open(FILE, ">$fname".".file_tag_wrong.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $file_tag_wrong);
close(FILE);

open(FILE, ">$fname".".error_map_file.log" ) or die "Can't open file $FILE: $!\n";
print(FILE $error_map_file);
close(FILE);
