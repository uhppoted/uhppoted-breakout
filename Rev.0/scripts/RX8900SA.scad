$fn=32;

body_width=5;
body_length=10.1;
body_height=3.2;
body_pins=14;

pin_length=1.2;
pin_width=0.43;
pin_height=0.23;
pin_separation=1.27;
pin_loft=0.8;
pin_overlap=0.1;
pin_offset=(body_length - pin_separation*((body_pins/2)-1))/2;

module pin() {
    l = pin_length/2;
    w = pin_width;
    h = pin_height;
    dl = pin_overlap;
    dh = pin_loft;
    r = 0.1;

    color("silver")
      translate([0,w/2,0])
      rotate(90,[1,0,0])
      linear_extrude(w)
      minkowski(){
        polygon([
          [-dl,dh+r],
          [l-h/2-h/2,dh+r],
          [l-h/2+r,r],
          [2*l-r,r],
          [2*l-r,h-r],
          [l+h/2-r,h-r],
          [l+h/2-h/2-2*r,dh+h-r],
          [-dl,dh+h-r],
          [-dl,dh+r]
          ]);
        circle(r);
      };
}

module left_pin() {
    translate([-body_width/2,pin_offset,0])
      mirror([1,0,0]) 
        pin();
}

module right_pin() {
    translate([+body_width/2,pin_offset,0])
        pin();
}

module body() {
    color("#444444")
    translate([0,body_length/2,pin_loft+pin_height/2]) {
      minkowski() {
        union() {
          linear_extrude(height=body_height-pin_loft-pin_height/2,scale=0.8)
            square([body_width-0.1,body_length-0.1],center=true);

          mirror([0,0,1])
            linear_extrude(height=pin_loft-0.1,scale=0.9)
              square([body_width-0.1,body_length-0.1],center=true);
        };
        sphere(0.1);
      };
   };
}

module pins() {
    for (pin=[1:1:body_pins/2]) {
        offset = (pin-1)*pin_separation;

        translate([0,offset,0])
          left_pin();

        translate([0,offset,0])
         right_pin();
    }
}

module label() {
    color("white")
    translate([0,body_length/2,body_height]) 
      linear_extrude(0.1)
      scale(0.1)
      rotate(-90,[0,0,1])
      text("RX8900SA",halign="center");
}

module dot() {
    color("#666666")
    translate([-1.25,body_length-1.75,body_height+0.1]) 
    cylinder(0.1,0.25,0.25, center=true);
}

module RX8900SA() {
    translate([0,-body_length/2,0])
      difference() {
        union() {
          body();
          pins();
          label();
        };
        dot();
     };
}

RX8900SA();
