function mm_from_inches(inches) = inches*25.5;


bearing_od = 22;
bearing_id   = 8;
bearing_w   = 7;

slidew =  mm_from_inches(.75);

runnerheight = mm_from_inches(3);
runnerdepth  = mm_from_inches(.75);
runnerwidth =  mm_from_inches(2.25);

module bearingnotch()
{
  cube(size =[bearing_w+ 2, mm_from_inches(1), bearing_od+6], center = true); 
}

module bolt()
{
	cylinder(h=100, r=3.5, center=true, $fs=1);
}

module bearing()
{
    color([0, 0, .7, 1]) {	
    difference() {
	cylinder(h=7, r=11, center=true, $fs=1);
	bolt();
   }
}
}

b1trans = [0, 2, runnerheight/2 - bearing_od/2];
b2trans = [0, 2, -runnerheight/2 + bearing_od/2];
b3trans = [slidew/2 + bearing_od/2, runnerdepth/2 + bearing_w/2, -runnerheight/2 + bearing_od];
b4trans = [-slidew/2 - bearing_od/2, runnerdepth/2 + bearing_w/2, -runnerheight/2 + bearing_od];
b5trans = [slidew/2 +  bearing_od/2, runnerdepth/2 + bearing_w/2, runnerheight/2 - bearing_od];
b6trans = [-slidew/2 - bearing_od/2, runnerdepth/2 + bearing_w/2, runnerheight/2 - bearing_od];

echo("======================================================");
echo(str("  Long holes @ ", runnerdepth/2 - b1trans[1], "mm from front, ", runnerheight/2- b1trans[2], "mm from top"));
echo(str("  Short holes @ ", runnerwidth/2 - b3trans[0], "mm from edge, ", runnerheight/2- b3trans[2], "mm from top"));
echo("======================================================");


translate(v=b1trans) rotate(90, [0,1, 0])  { bearing(); } 
translate(v=b2trans) rotate(90, [0,1, 0])  { bearing(); } 
translate(v=b3trans) rotate(90, [1,0, 0])  { bearing(); } 
translate(v=b4trans) rotate(90, [1,0, 0])  { bearing(); } 
translate(v=b5trans) rotate(90, [1,0, 0])  { bearing(); } 
translate(v=b6trans) rotate(90, [1,0, 0])  { bearing(); } 

	

module body()
{
          offset = 2 + runnerheight/2 - bearing_od/2;

	difference() {
		cube(size =[runnerwidth, runnerdepth, runnerheight], center = true);
		translate(v = [0, 0, offset] ) { bearingnotch(); }
		translate(v = [0, 0, -offset] ) { bearingnotch(); }
	
		//drill holes
		translate(v=b1trans) rotate(90, [0,1, 0])  { bolt(); } 
		translate(v=b2trans) rotate(90, [0,1, 0])  { bolt(); } 
		translate(v=b3trans) rotate(90, [1,0, 0])  { bolt(); } 
		translate(v=b4trans) rotate(90, [1,0, 0])  { bolt(); } 
		translate(v=b5trans) rotate(90, [1,0, 0])  { bolt(); } 
		translate(v=b6trans) rotate(90, [1,0, 0])  { bolt(); } 

	}
}

body();