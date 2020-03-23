# R3D
R3D (<i>Rendara3D</i>) is a graphics/physics engine that I am writing for learning purposes.
It is written in C++ and OpenGL.

<img src="/img/r3d_stack.gif?raw=true" width="300px"> <img src="/img/r3d_shoot.gif?raw=true" width="300px">

Features include
<ul>
<li> Instanced rendering, Shadows, Bloom, HDR.
<li> Rigid-body physics: discrete collision detection and response.
<li> Manifold generation using the Separating Axis Theorem
<li> Box stacking
<li> Entity-Component-System (ECS) based on archetypes for looping selected entities in a cache-friendly way
<li> <a href="https://github.com/ocornut/imgui"><font color="blue">dear ImGui</font></a> interface for editing shaders at run-time, changing settings and debugging.
</ul>

<img src="/img/r3d_overview.gif?raw=true" width="800px">

I have studied several resources online for writing Rendara3D.
For physics: Erin Catto's <a href="https://github.com/ocornut/imgui"><font color="blue">Box2D-lite</font></a>, Dirk Gregorius' talks at the <a href="https://www.gdcvault.com/search.php#&conference_id=&category=free&firstfocus=&keyword=dirk+gregorius"><font color="blue">GDC</font></a> and his  <a href="https://www.gamedev.net/profile/30524-dirk-gregorius/"><font color="blue">gamedev.net</font></a> posts.
For rendering: Joey de Vries's  <a href="https://learnopengl.com/"><font color="blue">learnopengl.com</font></a>, TheCherno's <a href="https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT"><font color="blue">videos</font></a> and ThinMatrix's <a href="https://www.youtube.com/watch?v=VS8wlS9hF8E&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP"><font color="blue">videos</font></a>.
For the ECS architecture: skypjacks's <a href="https://skypjack.github.io/tags/#ecs"><font color="blue">blog</font></a> and the bitsquid <a href="http://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html"><font color="blue">blog</font></a>.
