#!/bin/ksh93

typeset REPODIR="$HOME/apt/dists/stable/main/binary"

function distrib_package {
	(cd $1 
	 equivs-build $2 
	 ls -althr ${2}_*.deb

	 if [[ ! -d "$REPODIR/" ]]; then
	 	print "target repository directory does not exist"
	 	exit
	 fi
	 if [[ "$(which gpg)" = "" ]]; then print "\- \`gpg' not found"; exit; fi
	 if [[ "$(which apt-ftparchive)" = "" ]]; then print "\- \`apt-ftparchive' not found"; exit; fi

	 cp -prv ${2}_*.deb $REPODIR/

	 cd $REPODIR/

	 sudo dpkg-sig --sign builder ${2}_*.deb
	)
}

function sign_all {
	(cd $REPODIR/

	 apt-ftparchive packages . > Packages
	 gzip -c Packages > Packages.gz

	 apt-ftparchive release . > Release
	 gpg --clearsign -o InRelease Release
	 gpg -abs -o Release.gpg Release
	)
}
