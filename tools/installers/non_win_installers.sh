#!/bin/bash

clean() {
  if [[ -e ms_tmp_install_dir ]]
    then
  rm -rf ms_tmp_install_dir
  fi
  mkdir ms_tmp_install_dir
}

if [ ! make ]
  then
  echo "You must run this script from your build directory"
  exit
fi

if [[ ! `which rpm` ]]
  then
  echo "please install rpmbuild package"
  exit
fi

if [[ ! `which fpm` ]]
  then
  echo "please install fpm package (sudo gem install fpm)"
  exit
fi

if [ -z $1 ]
  then
  echo "Type the Major Version Number, followed by [ENTER]:"
  read major
fi

if [ -z $1 ]
  then
    echo "Type the Minor Version Number, followed by [ENTER]:"
    read minor
fi

if [ -z $1 ]
  then
    echo "Type the patch Number, followed by [ENTER]:"
    read patch
fi

echo "About to create dev and debug (symbols included versions of maidsafe)"
echo "Major version $major $1 Minor Version $minor $2 Patch Version $patch $3"
echo "please type (exactly) 'yesplease' to continue"
read confirm

if [ $confirm != "yesplease" ]
  then
  exit
fi

source ./CMakeCache.txt &>/dev/null

if [[ $(grep -i ubuntu /etc/issue) || $(grep -i debian /etc/issue) || $(grep -i mint /etc/issue) ]]
  then
  cmake . -DCMAKE_BUILD_TYPE="Debug"
  clean
  make -j8 DESTDIR=$PWD/ms_tmp_install_dir/ install
  fpm -s dir -t deb -C $PWD/ms_tmp_install_dir --name maidsafe-dev-dbg --version $major$1.$minor$2.$patch$3 --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d'git-all' -d 'libicu-dev' --description "MaidSafe Development Package with symbols"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' .

  cmake . -DCMAKE_BUILD_TYPE="Release"
  clean
  make -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  find ms_tmp_install_dir/ -type f -name "*.a" -exec strip {} \;
  fpm -s dir -t deb -C $PWD/ms_tmp_install_dir --name maidsafe-dev --version $major$1.$minor$2.$patch$3 --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d'git-all' -d 'libicu-dev' --description "MaidSafe Development Package"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' .

  clean
  make vault -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  make vault_manager -j8  DESTDIR=$PWD/ms_tmp_install_dir/ install
  find ms_tmp_install_dir/ -type f -name "*.a" -exec strip {} \;
  fpm -s dir -t deb -C $PWD/ms_tmp_install_dir --name maidsafe-farmer --version $major$1.$minor$2.$patch$3 --iteration 1 -d 'fuse' --description "MaidSafe Farmer Package"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' --after-install 'src/vault_manager/post_install' .
#also build release rpms
  clean
  make vault -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  make vault_manager -j8  DESTDIR=$PWD/ms_tmp_install_dir/ install
  find ms_tmp_install_dir/ -type f -name "*.a" -exec strip {} \;
  fpm -s dir -t rpm -C $PWD/ms_tmp_install_dir --name maidsafe-farmer --version $major$1.$minor$2.$patch$3 --iteration 1 -d 'fuse' --description "MaidSafe Farmer Package"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' --after-install 'src/vault_manager/post_install' .

elif [[ `grep -i fedora /etc/issue` || `grep -i redhat /etc/issue` || `grep -i suse /etc/issue` || `grep -i centos /etc/issue` ]]
then
  #we need an rpm system for the debug verions in particular
  cmake . -DCMAKE_BUILD_TYPE="Debug"
  clean
  make -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  fpm -s dir -t rpm -C $PWD/ms_tmp_install_dir --name maidsafe-dev-dbg --version $major$1.$minor$2.$patch$3 --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d'git-all' -d 'libicu-dev' --description "MaidSafe Development Package with symbols"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' .
  
  cmake . -DCMAKE_BUILD_TYPE="Release"
  clean
  make -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  find ms_tmp_install_dir/ -type f -name "*.a" -exec strip {} \;
  fpm -s dir -t rpm -C $PWD/ms_tmp_install_dir --name maidsafe-dev-dbg --version $major$1.$minor$2.$patch$3 --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d'git-all' -d 'libicu-dev' --description "MaidSafe Development Package"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' .

  clean
  make vault -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  make vault_manager -j8  DESTDIR=$PWD/ms_tmp_install_dir/ install
  find ms_tmp_install_dir/ -type f -name "*.a" -exec strip {} \;
  fpm -s dir -t rpm -C $PWD/ms_tmp_install_dir --name maidsafe-farmer --version $major$1.$minor$2.$patch$3 --iteration 1 -d 'fuse' --description "MaidSafe Farmer Package"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' --after-install 'src/vault_manager/post_install' .

elif [ "$(uname)" == "Darwin" ]
  then
  cmake . -DCMAKE_BUILD_TYPE="Debug"
  clean
  make -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  fpm -s dir -t osxpkg -C $PWD/ms_tmp_install_dir --name maidsafe-dev-dbg --version $major$1.$minor$2.$patch$3 --iteration 1 --description "MaidSafe Development Package (with symbols)"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' .

  cmake . -DCMAKE_BUILD_TYPE="Release"
  clean
  make -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  find ms_tmp_install_dir/ -type f -name "*.a" -exec strip {} \;
  fpm -s dir -t osxpkg -C $PWD/ms_tmp_install_dir --name maidsafe-dev --version $major$1.$minor$2.$patch$3 --iteration 1 --description "MaidSafe Development Package"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' .

  clean
  make vault -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  make vault_manager -j8  DESTDIR=$PWD/ms_tmp_install_dir/ install
  find ms_tmp_install_dir/ -type f -name "*.a" -exec strip {} \;
  fpm -s dir -t osxpkg -C $PWD/ms_tmp_install_dir --name maidsafe-farmer --version $major$1.$minor$2.$patch$3 --iteration 1 --description "MaidSafe Farmer Package"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' .
else
  cmake . -DCMAKE_BUILD_TYPE="Release"
  clean
  make vault -j8  DESTDIR="$PWD/ms_tmp_install_dir/" install
  make vault_manager -j8  DESTDIR=$PWD/ms_tmp_install_dir/ install
  find ms_tmp_install_dir/ -type f -name "*.a" -exec strip {} \;
  fpm -s dir -t tgz -C $PWD/ms_tmp_install_dir --name maidsafe-farmer --version $major$1.$minor$2.$patch$3 --iteration 1 -d 'fuse' --description "MaidSafe Farmer Package"  --maintainer 'dev@maidsafe.net' --url 'http://www.maidsafe.net' --license 'GPL' --vendor 'maidsafe.net limited' .
fi

# put things back as they were 
cmake . -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE"
make -j8 
