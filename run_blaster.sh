#!/bin/sh

cd $(dirname $0)/build
exec ./blaster run.frag &
