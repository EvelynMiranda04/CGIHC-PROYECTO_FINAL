

// ==========================================
// 4.2 - TEXTURAS Y MODELOS (Juan Pablo)
// ==========================================
// AVATAR
Model hw_cuerpo;
Model hw_cabeza;
Model hw_espada;
Model hw_PiernaDerecha;
Model hw_PiernaIzquierda;
Model hw_BrazoDerecho;
Model hw_BrazoIzquierdo;
// BANCA HW
Model hw_banca;
//Hornet HW
Model hornet;
//Cristal HW
Model hw_libros;
//Cornifer HW
Model cornifer;
//Poste HW
Model hw_poste;
//Sherma
Model hw_sherma;
//Shakra
Model hw_marissa;


	// -----------------------> 7.2.2 - Juan Pablo
	hw_cuerpo = Model();			hw_cuerpo.LoadModel("Models/hw_cuerpo.obj");
	hw_cabeza = Model();			hw_cabeza.LoadModel("Models/hw_cabeza.obj");
	hw_espada = Model();			hw_espada.LoadModel("Models/hw_espada.obj");
	hw_PiernaDerecha = Model();		hw_PiernaDerecha.LoadModel("Models/hw_PiernaDerecha.obj");
	hw_PiernaIzquierda = Model();	hw_PiernaIzquierda.LoadModel("Models/hw_PiernaIzquierda.obj");
	hw_BrazoDerecho = Model();		hw_BrazoDerecho.LoadModel("Models/hw_BrazoDerecho.obj");
	hw_BrazoIzquierdo = Model();	hw_BrazoIzquierdo.LoadModel("Models/hw_BrazoIzquierdo.obj");

	hw_banca.LoadModel("Models/hw_banca.obj");
	hornet.LoadModel("Models/hornet_mod_1.obj");
	hw_libros.LoadModel("Models/hw_libros.obj");
	cornifer.LoadModel("Models/hw_cornifer.obj");
	hw_poste.LoadModel("Models/hw_poste.obj");
	hw_sherma.LoadModel("Models/hw_sherma.obj");
	hw_marissa.LoadModel("Models/hw_marissa.obj");



		// >>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - JUAN PABLO >>>>>>>>>>>>>>>>>>>>>>>
		
		// === INSTANCIA HW - CUERPO (EL PADRE) ===
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(cuerpoPosX, 1.0f, cuerpoPosZ));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, cuerpoRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelCuerpo = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hw_cuerpo.RenderModel();

		// === INSTANCIA HW - CABEZA (HIJO DEL CUERPO) ===
		model = modelCuerpo; // Partimos de la base del cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_cabeza.RenderModel();

		// === INSTANCIA HW - ESPADA (HIJO DEL CUERPO) ===
		model = modelCuerpo; // Empezamos desde la posición del cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -0.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_espada.RenderModel();

		// === PIERNA DERECHA (Avanza 15°) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.18f, -0.6f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Tu rotación base
		model = glm::rotate(model, rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // +35°
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_PiernaDerecha.RenderModel();

		// === PIERNA IZQUIERDA (Retrocede -15°) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-0.18f, -0.6f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // -35°
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_PiernaIzquierda.RenderModel();

		// === BRAZO DERECHO (Retrocede con pierna derecha) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.37f, 0.2f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // -35°
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_BrazoDerecho.RenderModel();

		// === BRAZO IZQUIERDO (Avanza con pierna derecha) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-0.37f, 0.2f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // +35°
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_BrazoIzquierdo.RenderModel();

		// === INSTANCIAS DE HW ===
		// Banca
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(120.0f, 0.0f, -25.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_banca.RenderModel();

		//Poste 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(120.0f, 0.0f, -45.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_poste.RenderModel();

		
		// === Librerias 1-19  ===
		// INSTANCIA: Libreria 1
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -25.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 2
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -31.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 3
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -37.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 4
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -43.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 5
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -49.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 6
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -54.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 7
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -60.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 8
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -31.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  8
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  9
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -31.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  10
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -37.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  11
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -43.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  12
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -49.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  13
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -54.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  14
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -60.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 15
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(109.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 16
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(115.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 17
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(121.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 18
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(128.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 19
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(134.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();

		
		// === NPC HW ===
		//Hornet		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(130.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f)); // Escala inicial

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hornet.RenderModel();

		//Cornifer
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(110.0f, 0.0f, -30.0f));
		model = glm::rotate(model, 110 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cornifer.RenderModel();
		
		//Sherma
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(135.0f, -0.5f, -54.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Escala inicial
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_sherma.RenderModel();
		
		//Marrissa
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(110.0f, -0.5f, -54.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f)); // Escala inicial
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_marissa.RenderModel();
